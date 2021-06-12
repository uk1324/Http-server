#pragma once

#include <stdexcept>
#include <array>

#include "HttpRequest.h"

class ParserInterrupt : std::exception {};

// Up to date HTTP specifications: https://www.w3.org/Protocols/
// thing https://www.w3.org/Protocols/HTTP/Forum/

class HttpRequestParser
{
public:
	enum class Interrupt
	{
		reachedBufferEnd,
		finishedParsingFullRequest,
		unsupportedMethod,
		invalidChar,
		invalidHeaderValue,
		unsupportedVersion,
	};

	HttpRequestParser() = delete;
	HttpRequestParser(const char* buffer, int dataSize);

	void parse();

	bool finished() const;

	void setBufferSize(int size);

	HttpRequest result() const;
	HttpRequest& result();

	//Interrupt interruptCode();

private:
		static constexpr int hashMultiplier = 37;

	void parseRequestLine();

	void parseMethod();

	void parseRequestTarget();
	void parseVersion();

	void parseHeaderName();
	void parseHeaderValue();
	void parseCrlf();

	void parseHeaders();
	void parseHeaderLine();

	void parseBody();

	std::string parseUntil(char charToMatch);
	void skipChars(char charToMatch);
	void skipWhitespaces();
	void skipUntil(char charToMatch);
	bool matchString(std::string match);

	bool isValidPathChar(char chr);
	bool isValidHeaderChar(char chr);
	bool isValidSeparatorChar(char chr);
	int isWhitespace(char chr);
	int hexDigitToInt(char digit);

	void moveCurrentCharForward();

	static int hashChar(int lastHash, char chr);
	static int hashString(const char* string);

public:


private:
	enum class State
	{
		finishedParsing,
		parsingMethod,
		parsingRequestTarget,
		parsingVersion,

		parsingBody,
		parsingHeaders,

		parsingRequestLine,
		parsingHeaderName,
		parsingHeadersEndCrlf,
		parsingHeaderValue,
		parsingHeaderCrlf
		//parsingHeaders,
		//parsingBody
	};

	CaseInsensitiveString headerName;
	//bool parsedHeaderName = false;
	//bool parsedHeaderValue = false;
	std::string headerValue;
	std::string parsedCrlf;

	static constexpr int maxRequestSizeBytes = 8192;
	static constexpr int reservedStringSize = 16;


	int m_bufferSize;

	HttpRequest m_parsedRequest;

	HttpRequestParser::State m_currentState = State::parsingRequestLine;

	int bodyBytesParsed = 0;

	/*const char* const m_bufferStart;
	const char* const m_bufferEnd;*/
	const char* m_bufferStart;
	const char* m_bufferEnd;
	const char* m_currentChar;

	int m_bytesParsed = 0;

	bool m_finished = false;
};
inline bool HttpRequestParser::finished() const
{
	return m_finished;
}

inline void HttpRequestParser::setBufferSize(int size)
{
	m_bufferEnd = m_bufferStart + size;
	m_currentChar = m_bufferStart;
	//m_bufferSize = size;
}

inline HttpRequest HttpRequestParser::result() const
{
	return m_parsedRequest;
}

inline HttpRequest& HttpRequestParser::result()
{
	return m_parsedRequest;
}

//inline HttpRequestParser::InterruptCode HttpRequestParser::interruptCode()
//{
//	return m_interruptCode;
//}