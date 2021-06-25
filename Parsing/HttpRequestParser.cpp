#include "HttpRequestParser.h"

HttpRequestParser::HttpRequestParser(const char* buffer)
	: m_bufferStart(buffer)
	, m_currentChar(buffer)
	, m_bufferEnd(nullptr)
	, m_currentState(State::parsingRequestLine)
	, m_bytesParsed(0)
{}

void HttpRequestParser::parse(int messageSize)
{
	m_bufferEnd = m_bufferStart + messageSize;
	m_currentChar = m_bufferStart;

	try
	{
		while (true)
		{
			switch (m_currentState)
			{
			case State::parsingRequestLine:
				m_currentState = State::parsingRequestLine;
				parseRequestLine();

				checkIfRequestTooBig();

				[[fallthrough]];

			case State::parsingHeadersEndCrlf:
				m_currentState = State::parsingHeadersEndCrlf;
				try
				{
					parseCrlf();

					checkIfRequestTooBig();

					m_currentState = State::parsingBody;
					break;
				}
				catch (Interrupt interrupt)
				{
					if (interrupt == Interrupt::invalidChar)
					{
						m_currentChar -= parsedCrlfState.length();
						parsedCrlfState.clear();
					}
					else
					{
						throw interrupt;
					}
				}

				[[fallthrough]];

			case State::parsingHeaderName:

				m_currentState = State::parsingHeaderName;
				parseHeaderName();

				checkIfRequestTooBig();

				[[fallthrough]];

			case State::parsingHeaderValue:
				m_currentState = State::parsingHeaderValue;

				parseHeaderValue();

				checkIfRequestTooBig();

				[[fallthrough]];

			case State::parsingHeaderCrlf:
				m_currentState = State::parsingHeaderCrlf;
				parseCrlf();

				checkIfRequestTooBig();

				m_parsedRequest.headers[headerNameState] = headerValueState;
				headerNameState.clear();
				headerValueState.clear();
				parsedCrlfState.clear();

				m_currentState = State::parsingHeadersEndCrlf;
				break;

				[[fallthrough]];

			case State::parsingBody:
				m_currentState = State::parsingBody;

				try
				{
					parseBody();
				}
				catch (Interrupt interrupt)
				{
					throw interrupt;
				}
			}
		}
	}
	catch (Interrupt interrupt)
	{	 
		if ((interrupt != Interrupt::reachedBufferEnd) && (interrupt != Interrupt::finishedParsingFullRequest))
			resetState();

		throw interrupt;
	}
}

void HttpRequestParser::parseRequestLine()
{
	// Specification: https://datatracker.ietf.org/doc/html/rfc7230#section-3.1.1

	// Request line must be fully read by the first recv call
	try
	{
		parseMethod();
		parseRequestTarget();
		parseVersion();

		if (matchString("\r\n") == false)
			throw Interrupt::invalidChar;
	}
	catch (Interrupt interrupt)
	{
		throw interrupt;
	}
}

void HttpRequestParser::parseMethod()
{
	static const std::unordered_map<std::string, HttpMethod> methods
	{
		{ "GET",    HttpMethod::Get },
		{ "POST",   HttpMethod::Post },
		{ "PUT",    HttpMethod::Put },
		{ "DELETE", HttpMethod::Delete },
		{ "PATCH",  HttpMethod::Patch }
	};

	int parsedStringLength = 0;
	while (true)
	{
		if (m_currentChar[parsedStringLength] == ' ')
			break;

		// Buffer ended but haven't finished parsing
		if (m_currentChar + parsedStringLength == m_bufferEnd)
			throw Interrupt::invalidChar;

		parsedStringLength++;
	}

	std::string parsedString(m_currentChar, parsedStringLength);

	if (methods.count(parsedString) > 0)
	{
		m_parsedRequest.method = methods.at(parsedString);
		// + 1 to skip space
		moveCurrentCharForward(parsedString.length() + 1);
	}
	else
	{
		throw Interrupt::unsupportedMethod;
	}
}

void HttpRequestParser::parseRequestTarget()
{
	std::string requestTarget;

	while (true)
	{
		if (*m_currentChar == ' ')
		{
			if (requestTarget.length() > 0)
			{
				// Skip space
				moveCurrentCharForward();

				if (requestTarget.length() > maxUriLength)
					throw Interrupt::uriTooLong;

				m_parsedRequest.requestTarget = HttpRequestTarget(requestTarget);
				return;
			}
			else
			{
				throw Interrupt::invalidChar;
			}
		}

		// Buffer ended but haven't finished parsing
		if (m_currentChar == m_bufferEnd)
			throw Interrupt::invalidChar;

		requestTarget += *m_currentChar;
		moveCurrentCharForward();
	}
}

void HttpRequestParser::parseVersion()
{
	static const std::unordered_map<std::string, HttpVersion> versions = {
		{ "1.0", HttpVersion::Http10 },
		{ "1.1", HttpVersion::Http11 }
	};

	try
	{
		if (matchString("HTTP/") == false)
			throw Interrupt::invalidChar;
	}
	catch (Interrupt interrupt)
	{
		throw interrupt;
	}


	int parsedStringLength = 0;
	while (true)
	{
		if (m_currentChar[parsedStringLength] == '\r')
			break;

		// Buffer ended but haven't finished parsing
		if (m_currentChar + parsedStringLength == m_bufferEnd)
			throw Interrupt::invalidChar;

		parsedStringLength++;
	}

	std::string parsedString(m_currentChar, parsedStringLength);

	if (versions.count(parsedString) > 0)
	{
		m_parsedRequest.version = versions.at(parsedString);
		moveCurrentCharForward(parsedString.length());
	}
	else
	{
		throw Interrupt::unsupportedVersion;
	}
}
void HttpRequestParser::parseHeaderName()
{
	checkIfReachedBufferEnd();

	while (true)
	{
		if (*m_currentChar == ':')
		{
			// Skip colon
			moveCurrentCharForward();

			break;
		}
		else if (HttpParsingUtils::isValidHeaderNameChar(*m_currentChar) == false)
		{
			throw Interrupt::invalidChar;
		}

		headerNameState += *m_currentChar;
		moveCurrentCharForward();

		if (m_currentChar == m_bufferEnd)
		{
			throw Interrupt::reachedBufferEnd;
		}
	}
}

void HttpRequestParser::parseHeaderValue()
{
	checkIfReachedBufferEnd();

	if (headerValueState.length() == 0)
		skipWhitespaces();

	checkIfReachedBufferEnd();

	while (true)
	{
		if (*m_currentChar == '\r')
		{
			break;
		}
		else if (HttpParsingUtils::isValidHeaderValueChar(*m_currentChar) == false)
		{
			throw Interrupt::invalidChar;
		}

		headerValueState += *m_currentChar;
		moveCurrentCharForward();

		if (m_currentChar == m_bufferEnd)
		{
			throw Interrupt::reachedBufferEnd;
		}
	}
}

void HttpRequestParser::parseCrlf()
{
	static const std::string crlf = "\r\n";

	checkIfReachedBufferEnd();

	while (true)
	{
		if (*m_currentChar != crlf[parsedCrlfState.length()])
		{
			throw Interrupt::invalidChar;
		}

		moveCurrentCharForward();
		parsedCrlfState += *m_currentChar;

		if (parsedCrlfState.length() == crlf.length())
		{
			parsedCrlfState.clear();
			break;
		}

		if (m_currentChar > m_bufferEnd)
		{
			throw Interrupt::reachedBufferEnd;
		}
	}
}

void HttpRequestParser::parseBody()
{
	if (m_parsedRequest.headers.containsHeader("Content-Length"))
	{
		int contentLength;
		try
		{
			contentLength = m_parsedRequest.headers.tryGetHeaderInt("Content-Length");
			//contentLength = std::stoi(m_parsedRequest.headers["Content-Length"]);

			// Don't know if this is safe
			if (contentLength <= maxRequestBodyLength)
				m_parsedRequest.body.reserve(contentLength);
		}
		catch (std::exception error)
		{
			throw Interrupt::invalidHeaderValue;
		}

		int unreadBytesInBuffer = m_bufferEnd - m_currentChar;
		int bodyBytesRemeaining = contentLength - m_parsedRequest.body.length();
		// The first option is unlikely to happen but the client might send more bytes than Content-length specifies
		int bytesToRead = unreadBytesInBuffer > bodyBytesRemeaining
			? bodyBytesRemeaining
			: unreadBytesInBuffer;

		m_parsedRequest.body.append(m_currentChar, bytesToRead);
		moveCurrentCharForward(bytesToRead);

		if (m_parsedRequest.body.length() == contentLength)
			throw Interrupt::finishedParsingFullRequest;

		if (m_currentChar == m_bufferEnd)
			throw Interrupt::reachedBufferEnd;
	}
	else
	{
		throw Interrupt::finishedParsingFullRequest;
	}
}

void HttpRequestParser::skipWhitespaces()
{
	while ((HttpParsingUtils::isWhitespcae(*m_currentChar)) && (m_currentChar < m_bufferEnd))
		moveCurrentCharForward();
}

bool HttpRequestParser::matchString(std::string target)
{
	if (m_currentChar + target.length() - 1 > m_bufferEnd)
		throw Interrupt::invalidChar;

	for (size_t i = 0; i < target.length(); i++)
	{
		if (m_currentChar[i] != target[i])
		{
			m_currentChar += target.length();
			m_bytesParsed += target.length();
			return false;
		}
	}

	m_currentChar += target.length();
	m_bytesParsed += target.length();
	return true;
}

void HttpRequestParser::checkIfRequestTooBig()
{
	if (m_bytesParsed > maxRequestLength)
		throw Interrupt::requestTooBig;
}

void HttpRequestParser::checkIfReachedBufferEnd()
{
	if (m_currentChar == m_bufferEnd)
		throw Interrupt::reachedBufferEnd;
}