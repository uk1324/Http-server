#include "HttpRequestParser.h"

HttpRequestParser::HttpRequestParser(const char* buffer)
	: m_bufferStart(buffer)
	, m_currentChar(buffer)
	, m_bufferEnd(nullptr)
	, m_currentState(State::parsingRequestLine)
	, m_bytesParsed(0)
	, m_lastError()
{}

HttpRequestParser::Status HttpRequestParser::parse(int messageSize, HttpRequest& result)
{
	m_bufferEnd = m_bufferStart + messageSize;
	m_currentChar = m_bufferStart;

	// Could put the switch inside an if not this
	if (m_bytesParsed + messageSize > maxRequestLength)
	{
		resetState();
		m_lastError = Error::RequestTooBig;
		return Status::Error;
	}

	Status status = Status::Error;

	switch (m_currentState)
	{
		case State::parsingRequestLine:
			m_currentState = State::parsingRequestLine;
			status = parseRequestLine();
			if (status != Status::InternalFinished)
			{
				break;
			}
		[[fallthrough]];

		case State::parsingHeadersEndCrlf:
		caseParsingHeadersEndCrlf:
			m_currentState = State::parsingHeadersEndCrlf;

			status = parseCrlf();

			// Make a new function matchCrlf that will do this
			if (status == Status::Error && m_lastError == Error::InvalidCrlfChar)
			{
				m_currentChar -= parsedCrlfState.length();
				parsedCrlfState.clear();
			}
			else if (status == Status::InternalFinished)
			{
				goto caseParsingBody;
			}
			else
			{
				break;
			}
		[[fallthrough]];

		case State::parsingHeaderName:
			m_currentState = State::parsingHeaderName;

			status = parseHeaderName();

			if (status != Status::InternalFinished)
			{
				break;
			}
		[[fallthrough]];

		case State::parsingHeaderValue:
			m_currentState = State::parsingHeaderValue;

			status = parseHeaderValue();

			if (status != Status::InternalFinished)
			{
				break;
			}
		[[fallthrough]];

		case State::parsingHeaderCrlf:
			m_currentState = State::parsingHeaderCrlf;

			status = parseCrlf();

			if (status != Status::InternalFinished)
			{
				break;
			}

			m_parsedRequest.headers[headerNameState] = headerValueState;
			headerNameState.clear();
			headerValueState.clear();
			parsedCrlfState.clear();

			goto caseParsingHeadersEndCrlf;
		[[fallthrough]];

		case State::parsingBody:
		caseParsingBody:
			m_currentState = State::parsingBody;

			status = parseBody();
	}

	if (status == Status::Complete)
	{
		result = std::move(m_parsedRequest);
		resetState();
		return status;
	}
	else if (status != Status::Partial)
	{
		resetState();
		return status;
	}
	else
	{
		return status;
	}
}

HttpRequestParser::Status HttpRequestParser::parseRequestLine()
{
	// request-line = method SP request-target SP HTTP-version CRLF

	Status status;

	// The request line must be parsed in the first parse call
	status = parseMethod();
	if (status != Status::InternalFinished)
		return status;
	status = parseRequestTarget();
	if (status != Status::InternalFinished)
		return status;
	status = parseVersion();
	if (status != Status::InternalFinished)
		return status;

	if (matchString("\r\n") == false)
	{
		m_lastError = Error::InvalidCrlfChar;
		return Status::Error;
	}

	return status;
}

HttpRequestParser::Status HttpRequestParser::parseMethod()
{
	static const std::unordered_map<std::string, HttpMethod> methods
	{
		{ "GET",    HttpMethod::Get },
		{ "POST",   HttpMethod::Post },
		{ "PUT",    HttpMethod::Put },
		{ "DELETE", HttpMethod::Delete },
		{ "PATCH",  HttpMethod::Patch }
	};

	std::string parsedString;

	while (peek() != ' ')
	{
		// Reached end but haven't finished parsing
		if (isAtEnd())
		{
			m_lastError = Error::InvalidChar;
			return Status::Error;
		}

		parsedString += peek();
		advance();
	}

	// Skip space
	advance();

	if (methods.count(parsedString) > 0)
	{
		m_parsedRequest.method = methods.at(parsedString);
		return Status::InternalFinished;
	}
	else
	{
		m_lastError = Error::UnsupportedMethod;
		return Status::Error;
	}
}

HttpRequestParser::Status HttpRequestParser::parseRequestTarget()
{
	std::string requestTarget;

	while (true)
	{
		if (peek() == ' ')
		{
			if (requestTarget.length() > 0)
			{
				// Skip space
				advance();

				if (requestTarget.length() > maxUriLength)
				{
					m_lastError = Error::UriTooLong;
					return Status::Error;
				}

				m_parsedRequest.requestTarget = HttpRequestTarget(requestTarget);
				return Status::InternalFinished;
			}
			else
			{
				m_lastError = Error::InvalidChar;
				return Status::Error;
			}
		}

		// Reached end but haven't finished parsing
		if (isAtEnd())
		{
			m_lastError = Error::InvalidChar;
			return Status::Error;
		}
			
		requestTarget += peek();
		advance();
	}
}

HttpRequestParser::Status HttpRequestParser::parseVersion()
{
	static const std::unordered_map<std::string, HttpVersion> versions = {
		{ "1.0", HttpVersion::Http10 },
		{ "1.1", HttpVersion::Http11 }
	};

	if (matchString("HTTP/") == false)
	{
		m_lastError = Error::InvalidChar;
		return Status::Error;
	}
		
	std::string parsedString;

	while (peek() != '\r')
	{
		// Reached end but haven't finished parsing
		if (isAtEnd())
		{
			m_lastError = Error::InvalidChar;
			return Status::Error;
		}

		parsedString += peek();
		advance();
	}

	if (versions.count(parsedString) > 0)
	{
		m_parsedRequest.version = versions.at(parsedString);
		return Status::InternalFinished;
	}
	else
	{
		m_lastError = Error::UnsupportedVersion;
		return Status::Error;
	}
}

HttpRequestParser::Status HttpRequestParser::parseHeaderName()
{
	while (isAtEnd() == false)
	{
		if (peek() == ':')
		{
			// Skip colon
			advance();

			return Status::InternalFinished;
		}
		else if (HttpParsingUtils::isValidHeaderNameChar(peek()) == false)
		{
			m_lastError = Error::InvalidChar;
			return Status::Error;
		}

		headerNameState += peek();
		advance();
	}

	return Status::Partial;
}

HttpRequestParser::Status HttpRequestParser::parseHeaderValue()
{
	if (isAtEnd())
	{
		return Status::Partial;
	}

	if (headerValueState.length() == 0)
		skipWhitespaces();

	while (isAtEnd() == false)
	{
		if (peek() == '\r')
		{
			return Status::InternalFinished;
		}
		else if (HttpParsingUtils::isValidHeaderValueChar(peek()) == false)
		{
			m_lastError = Error::InvalidChar;
			return Status::Error;
		}

		headerValueState += peek();
		advance();
	}

	return Status::Partial;
}

HttpRequestParser::Status HttpRequestParser::parseCrlf()
{
	static const std::string crlf = "\r\n";

	if (isAtEnd())
	{
		return Status::Partial;
	}

	while (true)
	{
		if (peek() != crlf[parsedCrlfState.length()])
		{
			m_lastError = Error::InvalidCrlfChar;
			return Status::Error;
		}

		advance();
		parsedCrlfState += peek();

		if (parsedCrlfState.length() == crlf.length())
		{
			parsedCrlfState.clear();
			return Status::InternalFinished;
		}

		if (m_currentChar > m_bufferEnd)
		{
			return Status::Partial;
		}
	}
}

HttpRequestParser::Status HttpRequestParser::parseBody()
{
	if (m_parsedRequest.headers.containsHeader("Content-Length"))
	{
		int contentLength;
		try
		{
			contentLength = m_parsedRequest.headers.getHeaderInt("Content-Length");
		}
		catch (const std::invalid_argument&)
		{
			m_lastError = Error::InvalidHeaderValue;
			return Status::Error;
		}
		catch (const std::out_of_range&)
		{
			m_lastError = Error::InvalidHeaderValue;
			return Status::Error;
		}

		if (contentLength < 0)
		{
			m_lastError = Error::InvalidHeaderValue;
			return Status::Error;
		}

		// Don't know if this is safe
		if (contentLength <= maxRequestBodyLength)
			m_parsedRequest.body.reserve(contentLength);
		// should also add a case if body is too big
		// don't know if there even should a max body size or just max request size
		else if (m_bytesParsed + contentLength > maxRequestLength)
		{
			m_lastError = Error::RequestTooBig;
			return Status::Error;
		}

		int unreadBytesInBuffer = m_bufferEnd - m_currentChar;
		int bodyBytesRemeaining = contentLength - m_parsedRequest.body.length();
		// The first option is unlikely to happen but the client might send more bytes than Content-length specifies
		int bytesToRead = unreadBytesInBuffer > bodyBytesRemeaining
			? bodyBytesRemeaining
			: unreadBytesInBuffer;

		m_parsedRequest.body.append(m_currentChar, bytesToRead);
		advance(bytesToRead);

		if (m_parsedRequest.body.length() == contentLength)
		{
			return Status::Complete;
		}

		return Status::Partial;
	}
	else
	{
		return Status::Complete;
	}
}

void HttpRequestParser::skipWhitespaces()
{
	while ((HttpParsingUtils::isWhitespace(peek())) && (m_currentChar < m_bufferEnd))
		advance();
}

bool HttpRequestParser::matchString(std::string target)
{
	if (m_currentChar + target.length() - 1 > m_bufferEnd)
		return false;

	for (size_t i = 0; i < target.length(); i++)
	{
		if (m_currentChar[i] != target[i])
		{
			advance(target.length());
			return false;
		}
	}

	advance(target.length());
	return true;
}

HttpRequestParser::Error HttpRequestParser::lastError() const
{
	return m_lastError;
}
