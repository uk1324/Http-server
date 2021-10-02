#pragma once

#include <stdexcept>
#include <array>

#include "../Messages/HttpRequest.h"
#include "HttpParsingUtils.h"

// Up to date HTTP specifications: https://www.w3.org/Protocols/
// HTTP/1.1 Implementor's Forum:   https://www.w3.org/Protocols/HTTP/Forum/

// Check for size too big at start not while parsing
// Split information about parsing status to status and error enums
// Should function return value or set state
// What is good way to handle the state machine
// How to make it easy to parse both request and responses using this class
// Maybe optimize character checking using a lookup table instead of using if statements
// Reduce allocation better reset of internal request
// Change Interrupt names
// Cast to unsigend char before using functions like isalpha or just don't use them

class HttpRequestParser
{
public:
	HttpRequestParser() = delete;
	HttpRequestParser(const char* buffer);

	enum class [[nodiscard]] Status
	{
		Complete,
		Partial,
		Error,
		InternalFinished,
	};

	Status parse(int messageSize, HttpRequest& result);

	enum class Error
	{
		RequestTooBig,
		InvalidCrlfChar,
		InvalidChar,
		UnsupportedMethod,
		UnsupportedVersion,
		UriTooLong,
		InvalidHeaderValue
	};

	Error lastError() const;

private:
	Status parseRequestLine();
	Status parseMethod();
	Status parseRequestTarget();
	Status parseVersion();

	Status parseHeaderName();
	CaseInsensitiveString headerNameState;
	Status parseHeaderValue();
	std::string headerValueState;
	Status parseCrlf();
	std::string parsedCrlfState;

	Status parseBody();

	void skipWhitespaces();
	bool matchString(std::string match);

	void advance(int count);
	void advance();
	char peek();
	bool isAtEnd();

	void resetState();;

private:
	enum class State
	{
		parsingRequestLine,
		parsingHeaderName,
		parsingHeadersEndCrlf,
		parsingHeaderValue,
		parsingHeaderCrlf,
		parsingBody
	};

	static constexpr int maxRequestLength = 8192;
	static constexpr int maxRequestBodyLength = 8192;
	static constexpr int maxUriLength = 1024;

	HttpRequest m_parsedRequest;

	State m_currentState;
	Error m_lastError;

	const char* m_bufferStart;
	const char* m_bufferEnd;
	const char* m_currentChar;

	int m_bytesParsed;
};

inline void HttpRequestParser::advance(int count)
{
	m_currentChar += count;
	m_bytesParsed += count;
}

inline void HttpRequestParser::advance()
{
	m_currentChar++;
	m_bytesParsed++;
}

inline char HttpRequestParser::peek()
{
	return *m_currentChar;
}

inline bool HttpRequestParser::isAtEnd()
{
	return m_currentChar >= m_bufferEnd;
}

inline void HttpRequestParser::resetState()
{
	m_bytesParsed = 0;
	m_parsedRequest = HttpRequest();
	m_currentState = State::parsingRequestLine;
	//m_lastError = Error::None;
	// This may be unsafe because std::string::clear() does not dealocate memory it just sets the length to 0
	headerNameState.clear();
	headerValueState.clear();
	parsedCrlfState.clear();
}

