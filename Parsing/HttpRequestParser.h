#pragma once

#include <stdexcept>
#include <array>

#include "../HttpRequest.h"

// Up to date HTTP specifications: https://www.w3.org/Protocols/
// HTTP/1.1 Implementor's Forum:   https://www.w3.org/Protocols/HTTP/Forum/

class HttpRequestParser
{
public:
	enum class Interrupt
	{
		reachedBufferEnd,
		finishedParsingFullRequest,
		invalidChar,
		invalidHeaderValue,
		unsupportedMethod,
		unsupportedVersion,
		requestTooBig,
	};

	HttpRequestParser() = delete;
	HttpRequestParser(const char* buffer);

	void parse(int messageSize);

	inline HttpRequest result() const;
	inline HttpRequest& result();

private:
	void parseRequestLine();
	void parseMethod();
	void parseRequestTarget();
	void parseVersion();

	void parseHeaderName();
	void parseHeaderValue();
	void parseCrlf();
	CaseInsensitiveString headerNameState;
	std::string headerValueState;
	std::string parsedCrlfState;

	void parseBody();

	void skipChars(char charToMatch);
	void skipWhitespaces();
	void skipUntil(char charToMatch);
	bool matchString(std::string match);

	bool isValidPathChar(char chr);
	bool isValidHeaderChar(char chr);
	bool isValidSeparatorChar(char chr);
	int isWhitespace(char chr);
	int hexDigitToInt(char digit);

	inline void moveCurrentCharForward();
	inline void moveCurrentCharForward(int count);

	void resetState();

	void checkIfRequestTooBig();
	void checkIfReachBufferEnd();

	static int hashChar(int lastHash, char chr);
	static int hashString(const char* string);

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

	HttpRequest m_parsedRequest;

	State m_currentState = State::parsingRequestLine;

	const char* m_bufferStart;
	const char* m_bufferEnd;
	const char* m_currentChar;

	int m_bytesParsed = 0;
};

inline HttpRequest HttpRequestParser::result() const
{
	return m_parsedRequest;
}

inline HttpRequest& HttpRequestParser::result()
{
	return m_parsedRequest;
}

inline void HttpRequestParser::moveCurrentCharForward()
{
	m_currentChar++;
	m_bytesParsed++;
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

