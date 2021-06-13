#include "HttpRequestParser.h"




#include <iostream>


HttpRequestParser::HttpRequestParser(const char* buffer)
	: m_bufferStart(buffer)
	, m_currentChar(buffer)
	, m_bufferEnd(nullptr)
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
	static const int hashes[] = {
		hashString("GET"),
		hashString("HEAD"),
		hashString("POST"),
		hashString("PUT"),
		hashString("DELETE"),
		hashString("TRACE"),
		hashString("OPTIONS"),
		hashString("CONNECT"),
		hashString("PATCH")
	};
	static constexpr int longestMethodName = 7;

	int hash = 0;
	int hashedCharsCount = 0;

	while (true)
	{
		if (*m_currentChar == ' ')
		{
			// Skip space
			moveCurrentCharForward();

			break;
		}

		// Buffer ended but haven't finished parsing
		if (m_currentChar == m_bufferEnd)
		{
			throw Interrupt::invalidChar;
		}

		hash = hashChar(hash, *m_currentChar);
		hashedCharsCount++;
		moveCurrentCharForward();

		if (hashedCharsCount > longestMethodName)
		{
			throw Interrupt::unsupportedMethod;
		}
	}

	for (int i = 0; i < sizeof(hashes) / sizeof(hashes[0]); i++)
	{
		if (hash == hashes[i])
		{
			m_parsedRequest.method = static_cast<HttpMethod>(i);
			return;
		}
	}

	throw Interrupt::unsupportedMethod;
}

void HttpRequestParser::parseRequestTarget()
{
	// Only allow relative URLs
	if (*m_currentChar != '/')
		throw Interrupt::invalidChar;

	while (true)
	{
		if (*m_currentChar == ' ')
		{
			if (m_parsedRequest.requestTarget.length() > 0)
			{
				// Skip space
				moveCurrentCharForward();

				return;
			}
			else
			{
				throw Interrupt::invalidChar;
			}
		}

		// Buffer ended but haven't finished parsing or invalid char
		if ((isValidPathChar(*m_currentChar) == false) || (m_currentChar == m_bufferEnd))
		{
			throw Interrupt::invalidChar;
		}

		m_parsedRequest.requestTarget += *m_currentChar;
		moveCurrentCharForward();
	}
}

void HttpRequestParser::parseVersion()
{
	static const int hashes[] = {
		hashString("1.0"),
		hashString("1.1"),
	};

	static constexpr int maxVersionStringLength = 3;

	try
	{
		if (matchString("HTTP/") == false)
			throw Interrupt::invalidChar;
	}
	catch (Interrupt interrupt)
	{
		throw interrupt;
	}

	if (m_currentChar + maxVersionStringLength > m_bufferEnd)
	{
		throw Interrupt::invalidChar;
	}

	int hash = 0;
	int hashedCharsCount = 0;

	while (true)
	{
		if (*m_currentChar == '\r')
			break;
		else if (m_currentChar == m_bufferEnd)
			throw Interrupt::unsupportedVersion;

		hash = hashChar(hash, *m_currentChar);
		hashedCharsCount++;

		moveCurrentCharForward();

		if (hashedCharsCount > maxVersionStringLength)
			throw Interrupt::unsupportedVersion;
	}

	for (int i = 0; i < sizeof(hashes) / sizeof(hashes[0]); i++)
	{
		if (hash == hashes[i])
		{
			m_parsedRequest.version = static_cast<HttpVersion>(i);
			return;
		}
	}

	throw Interrupt::unsupportedVersion;;
}
void HttpRequestParser::parseHeaderName()
{
	while (true)
	{
		if (*m_currentChar == ':')
		{
			// Skip colon
			moveCurrentCharForward();

			break;
		}
		else if (isValidHeaderChar(*m_currentChar) == false)
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
	while (true)
	{
		if (*m_currentChar == '\r')
		{
			break;
		}
		else if (((isValidHeaderChar(*m_currentChar) == false)
			&& (isWhitespace(*m_currentChar) == false)
			&& (isValidSeparatorChar(*m_currentChar) == false)))
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
	if (m_parsedRequest.headers.count("Content-Length") > 0)
	{
		int contentLength;
		try
		{
			contentLength = std::stoi(m_parsedRequest.headers["Content-Length"]);

			// Don't know if this is safe
			if (contentLength <= maxRequestBodyLength)
				m_parsedRequest.body.reserve(contentLength);
		}
		catch (std::exception error)
		{
			throw Interrupt::invalidHeaderValue;
		}

		while (true)
		{
			m_parsedRequest.body += *m_currentChar;
			moveCurrentCharForward();

			if (m_parsedRequest.body.length() == contentLength)
			{
				throw Interrupt::finishedParsingFullRequest;
			}

			if (m_currentChar == m_bufferEnd)
			{
				throw Interrupt::reachedBufferEnd;
			}
		}
	}
	else
	{
		throw Interrupt::finishedParsingFullRequest;
	}
}

void HttpRequestParser::skipChars(char charToMatch)
{
	while ((*m_currentChar == charToMatch) && (m_currentChar < m_bufferEnd))
		moveCurrentCharForward();
}

void HttpRequestParser::skipWhitespaces()
{
	while ((isWhitespace(*m_currentChar)) && (m_currentChar < m_bufferEnd))
		moveCurrentCharForward();
}

void HttpRequestParser::skipUntil(char charToMatch)
{
	while ((*m_currentChar != charToMatch) && (m_currentChar < m_bufferEnd))
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

bool HttpRequestParser::isValidPathChar(char chr)
{
	// https://datatracker.ietf.org/doc/html/rfc3986#section-2
	return isalnum(chr)
		|| chr == '-'
		|| chr == '.'
		|| chr == '_'
		|| chr == '~'
		|| chr == ':'
		|| chr == '/'
		|| chr == '?'
		|| chr == '#'
		|| chr == '['
		|| chr == ']'
		|| chr == '@'
		|| chr == '!'
		|| chr == '$'
		|| chr == '&'
		|| chr == '\''
		|| chr == '('
		|| chr == ')'
		|| chr == '*'
		|| chr == '+'
		|| chr == ','
		|| chr == ';'
		|| chr == '=';
}

bool HttpRequestParser::isValidHeaderChar(char chr)
{
	// Specification: https://datatracker.ietf.org/doc/html/rfc7230#section-3.2.6
	return isalnum(chr)
		|| chr == '!'
		|| chr == '#'
		|| chr == '$'
		|| chr == '%'
		|| chr == '&'
		|| chr == '\''
		|| chr == '*'
		|| chr == '+'
		|| chr == '-'
		|| chr == '.'
		|| chr == '^'
		|| chr == '_'
		|| chr == '`'
		|| chr == '|'
		|| chr == '~';
}

bool HttpRequestParser::isValidSeparatorChar(char chr)
{
	return chr == '('
		|| chr == ')'
		|| chr == '/'
		|| chr == ','
		|| chr == ':'
		|| chr == '<'
		|| chr == '='
		|| chr == '>'
		|| chr == '?'
		|| chr == '@'
		|| chr == '['
		|| chr == ']'
		|| chr == '\\'
		|| chr == '{'
		|| chr == '}'
		|| chr == ';'
		|| chr == '"';

}

int HttpRequestParser::isWhitespace(char chr)
{
	return chr == ' ' || chr == '\t';
}

int HttpRequestParser::hexDigitToInt(char digit)
{
	int num = -1;
	if ((digit >= '0') && (digit <= '9'))
		num = (digit - '0');
	else if ((digit >= 'A') && (digit <= 'F'))
		num = (digit - 'A' + 10);
	else if ((digit >= 'a') && (digit <= 'f'))
		num = (digit - 'a' + 10);
	return num;
}

void HttpRequestParser::checkIfRequestTooBig()
{
	if (m_bytesParsed > maxRequestLength)
		throw Interrupt::requestTooBig;
}

int HttpRequestParser::hashChar(int lastHash, char chr)
{
	static constexpr int hashMultiplier = 37;
	return lastHash * hashMultiplier + chr;
}

int HttpRequestParser::hashString(const char* string)
{
	int hash = 0;
	for (const char* chr = string; *chr != '\0'; chr++)
	{
		hash = hashChar(hash, *chr);
	}

	return hash;
}
