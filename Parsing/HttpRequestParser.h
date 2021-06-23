#pragma once

#include <stdexcept>
#include <array>

#include "../HttpRequest.h"
#include "../HttpParsingUtils.h"

// Up to date HTTP specifications: https://www.w3.org/Protocols/
// HTTP/1.1 Implementor's Forum:   https://www.w3.org/Protocols/HTTP/Forum/

class HttpRequestParser
{
public:
	HttpRequestParser() = delete;
	HttpRequestParser(const char* buffer);

	void parse(int messageSize);

	inline HttpRequest result() const;
	inline HttpRequest& result();

	enum class Interrupt
	{
		reachedBufferEnd,
		finishedParsingFullRequest,
		invalidChar,
		invalidHeaderValue,
		unsupportedMethod,
		unsupportedVersion,
		requestTooBig,
		uriTooLong
	};

private:
	void parseRequestLine();
	void parseMethod();
	void parseRequestTarget();
	void parseVersion();

	void parseHeaderName();
	CaseInsensitiveString headerNameState;
	void parseHeaderValue();
	std::string headerValueState;
	void parseCrlf();
	std::string parsedCrlfState;

	void parseBody();

	void skipWhitespaces();
	bool matchString(std::string match);

	inline void moveCurrentCharForward(int count = 1);

	void resetState();

	void checkIfRequestTooBig();
	void checkIfReachedBufferEnd();

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

	const char* m_bufferStart;
	const char* m_bufferEnd;
	const char* m_currentChar;

	int m_bytesParsed;
};

inline HttpRequest HttpRequestParser::result() const
{
	return m_parsedRequest;
}

inline HttpRequest& HttpRequestParser::result()
{
	return m_parsedRequest;
}

inline void HttpRequestParser::moveCurrentCharForward(int count)
{
	m_currentChar += count;
	m_bytesParsed += count;
}

inline void HttpRequestParser::resetState()
{
	m_bytesParsed = 0;
	m_parsedRequest = HttpRequest();
	// This may be unsafe because std::string::clear() does not dealocate memory it just sets the length to 0
	headerNameState.clear();
	headerValueState.clear();
	parsedCrlfState.clear();
}

