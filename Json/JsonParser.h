#pragma once

// mabe use an annymous namespace
// store parse method in json namespace 
// rename json object to value

// JSON RFC - https://datatracker.ietf.org/doc/html/rfc8259
// UTF8 RFC - https://datatracker.ietf.org/doc/html/rfc3629

// Why unsigned chars are used - look at notes https://en.cppreference.com/w/cpp/string/byte/isalpha

// Maybe optimize it by removing the scanner
// Faster keyword matching
// Unchecked peek() fucntion

// do while for parsing lists??
//if (!check(TOKEN_RIGHT_PAREN)) {
//	do {
//		current->function->arity++;
//		if (current->function->arity > 255) {
//			errorAtCurrent("Can't have more than 255 parameters.");
//		}
//		uint8_t constant = parseVariable("Expect parameter name.");
//		defineVariable(constant);
//	} while (match(TOKEN_COMMA));
//}

#include <string>
#include <stdexcept>
#include <vector>
#include <unordered_map>
#include <algorithm>
#include <optional>

#include "Json.h"
#include "../Utils/assertions.h"

class JsonParser
{
public:
	static Json parse(const std::string& string);

	class ParsingError : public std::runtime_error
	{
	public:
		ParsingError(const char* what, size_t charIndex);
		size_t charIndex() const;
	private:
		const size_t m_charIndex;
	};

private:
	JsonParser(const std::string& string, Json& object);

	void parse();

	static constexpr int MAX_NESTING_DEPTH = 64;

	enum class TokenType
	{
		LeftParen,
		RightParen,
		LeftBracket,
		RightBracket,
		Colon,
		Comma,
		String,
		Number,
		True,
		False,
		Null
	};

	struct Token
	{
		Token(TokenType type, const std::string& string, size_t index);
		Token(TokenType type, std::string&& string, size_t index) noexcept;
		Token(TokenType type, size_t index);

		// For debugging.
		std::string toString() const;

		const TokenType type;
		// Maybe use std::optional
		// Stores the value if type is Number or String
		const std::string value;
		// For debugging.
		const size_t index;
	};

private:
	Json object();
	Json array();
	Json value();

	void advance();
	const Token& consume(TokenType token, const char* errorMessage);
	bool check(TokenType token) const;
	bool match(TokenType token);
	const Token& peek() const;
	const Token& peekPrevious() const;
	bool isAtEnd() const;

	// Prevents errors about not all code paths returning a value.
	[[noreturn]] void error(const char* message) const;

private:
	size_t m_current;
	int m_nestingDepth;
	Json& m_object;
	const std::string& m_string;
	std::vector<Token> m_tokens;

private:
	class Scanner
	{
	public:
		Scanner(const std::string& string, std::vector<JsonParser::Token>& tokens);
		void scanTokens();

	private:
		void scanToken();

		void string();

		void consumeUtf8Char(std::string& string);
		std::optional<int> countRequiredOctets(char chr);
		bool isValidUnescapedAsciiChar(char chr);

		void consumeEscapedChar(std::string& string);
		std::optional<int> hexDigitToInt(char chr);

		void number();
		void keyword();

		void advance();
		unsigned char peek() const;
		unsigned char peekPrevious() const;
		bool match(char chr);
		bool checkPrevious(char chr) const;
		bool isAtEnd() const;

		// Prevents errors about not all code paths returning a value.
		[[noreturn]] void error(const char* message) const;

		void addToken(TokenType type);
		void addToken(TokenType type, const std::string& string);
		void addToken(TokenType type, std::string&& string);

	private:
		size_t m_current;
		size_t m_start;
		const std::string& m_string;
		std::vector<JsonParser::Token>& m_tokens;
	};
};

