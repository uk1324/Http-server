#include "JsonParser.h"

JsonParser::Scanner::Scanner(const std::string& string, std::vector<JsonParser::Token>& tokens)
	: m_current(0)
	, m_start(m_current)
	, m_string(string)
	, m_tokens(tokens)
{}

void JsonParser::Scanner::scanTokens()
{
	m_current = 0;

	while (isAtEnd() == false)
	{
		m_start = m_current;
		scanToken();
	}
}

void JsonParser::Scanner::scanToken()
{
	unsigned char chr = peek();
	advance();

	switch (chr)
	{
	case '{': addToken(TokenType::LeftParen); break;
	case '}': addToken(TokenType::RightParen); break;
	case '[': addToken(TokenType::LeftBracket); break;
	case ']': addToken(TokenType::RightBracket); break;
	case ':': addToken(TokenType::Colon); break;
	case ',': addToken(TokenType::Comma); break;

	case ' ':
	case '\t':
	case '\r':
	case '\n':
		break;

	case '"':
		string();
		break;

	case '-':
		advance();
		number();
		break;

	default:
		if (isdigit(chr))
		{
			number();
			break;
		}
		else if (isalpha(chr))
		{
			keyword();
			break;
		}
		
		error("Illegal character");
	}
}

void JsonParser::Scanner::string()
{
	std::string string;

	while (peek() != '"')
	{
		if (match('\\'))
		{
			consumeEscapedChar(string);
		}
		else
		{
			consumeUtf8Char(string);
		}
	}

	addToken(TokenType::String, std::move(string));

	// Skip '"'
	advance();
}

void JsonParser::Scanner::consumeUtf8Char(std::string& string)
{
	// Char. number range  |        UTF-8 octet sequence
	//    (hexadecimal)    |              (binary)
	// --------------------+---------------------------------------------
	// 0000 0000-0000 007F | 0xxxxxxx
	// 0000 0080-0000 07FF | 110xxxxx 10xxxxxx
	// 0000 0800-0000 FFFF | 1110xxxx 10xxxxxx 10xxxxxx
	// 0001 0000-0010 FFFF | 11110xxx 10xxxxxx 10xxxxxx 10xxxxxx

	// There are more invalid UTF-8 sequences like the ones reserved for UTF-16 but I don't want to deal with that.

	auto octets = countRequiredOctets(peek());

	if (octets.has_value() == false)
		error("Invalid UTF-8 sequence");

	if ((octets.value() == 0) && (isValidUnescapedAsciiChar(peek()) == false))
		error("Invalid unescaped char");

	string += peek();
	advance();

	for (int i = 0; i < octets.value(); i++)
	{
		char chr = peek();
		if ((chr & 0b11000000) != 0b10000000)
			error("Invalid UTF-8 sequence");
		string += chr;
		advance();
	}
}

std::optional<int> JsonParser::Scanner::countRequiredOctets(char c)
{
	// ASCII
	if ((c >= 0x00) && (c <= 0x7F))
		return 0;

	// ANDing with required bits + 1 so 5 leading bits are not allowed.

	if ((c & 0b11111000) == 0b11110000)
		return 3;
	if ((c & 0b11110000) == 0b11100000)
		return 2;
	if ((c & 0b11100000) == 0b11000000)
		return 1;

	return std::nullopt;
}

void JsonParser::Scanner::consumeEscapedChar(std::string& string)
{
	char chr = peek();
	advance();

	switch (chr)
	{
	case '"':  string += '"';  break;
	case '\\': string += '\\'; break;
	case '/':  string += '/';  break;
	case 'b':  string += '\b'; break;
	case 'f':  string += '\f'; break;
	case 'n':  string += '\n'; break;
	case 'r':  string += '\r'; break;
	case 't':  string += '\t'; break;
	case 'u': 
		for (int i = 0; i < 2; i++)
		{
			auto digit1 = hexDigitToInt(peek());
			auto digit2 = hexDigitToInt(peek());

			if ((digit1.has_value() == false) || (digit2.has_value() == false))
				error("Invalid character inside escaped sequence");

			string += static_cast<char>(digit1.value() * 0x10 + digit2.value());

			advance();
			advance();
		}

		break;
	default:
		error("Invalid character inside escaped sequence");
	}
}

std::optional<int> JsonParser::Scanner::hexDigitToInt(char chr)
{
	if ((chr >= 'A') && (chr <= 'F'))
	{
		return chr - 'A' + 10;
	}
	else if ((chr >= 'a') && (chr <= 'f'))
	{
		return chr - 'a' + 10;
	}
	else if ((chr >= '0') && (chr <= '9'))
	{
		return chr - '0';
	}
	else
	{
		return std::nullopt;
	}
}

void JsonParser::Scanner::number()
{
	// Numbers can be at the end of the file so I had to add the isAtEnd() checks.

	if (isdigit(peekPrevious()) == false)
		error("A number can't just be a minus sign");

	if (checkPrevious('0'))
	{
		if ((isAtEnd() == false) && (isdigit(peek())))
			error("Leading zeros not allowed");
	}
	else
	{
		while ((isAtEnd() == false) && (isdigit(peek())))
		{
			advance();
		}
	}

	if ((isAtEnd() == false) && (match('.')))
	{
		if (isdigit(peek()) == false)
			error("Number literals can't end with a dot");

		while ((isAtEnd() == false) && isdigit(peek()))
		{
			advance();
		}
	}

	if ((isAtEnd() == false) && (match('e') || match('E')))
	{
		if ((match('-') || match('+')) && (isdigit(peek()) == false))
			error("Number literals can't end with a '-' or '+'");
		else if (isdigit(peek()) == false)
			error("Number literals can't end with an 'E'");

		while ((isAtEnd() == false) && (isdigit(peek())))
		{
			advance();
		}
	}

	std::string string = m_string.substr(m_start, m_current - m_start);
	addToken(TokenType::Number, std::move(string));
}

void JsonParser::Scanner::keyword()
{
	static const std::unordered_map<std::string, TokenType> keywords = {
		{"true",  TokenType::True},
		{"false", TokenType::False},
		{"null",  TokenType::Null},
	};

	static const size_t longestKeywordLength = std::max_element(keywords.begin(), keywords.end(),
		[](const auto& p1, const auto& p2) { return p1.first.length() < p2.first.length(); }
	)->first.length();

	while ((isAtEnd() == false) && (isalpha(peek())))
	{
		advance();
	}

	if ((m_current - m_start) > longestKeywordLength)
		error("Not a keyword");

	std::string keyword = m_string.substr(m_start, m_current - m_start);

	if (keywords.count(keyword) > 0)
	{
		TokenType type = keywords.at(keyword);
		addToken(type);
	}
	else
	{
		error("Not a keyword");
	}
}

void JsonParser::Scanner::advance()
{
	m_current++;
}

unsigned char JsonParser::Scanner::peek() const
{
	if (isAtEnd())
		error("Unexpected EOF");
	else
		return m_string[m_current];
}

unsigned char JsonParser::Scanner::peekPrevious() const
{
	if ((m_current - 1) >= m_string.length())
		error("Unexpected EOF");
	else
		return m_string[m_current - 1];
}

bool JsonParser::Scanner::match(char chr)
{
	if (peek() == chr)
	{
		advance();
		return true;
	}
	else
	{
		return false;
	}
}

bool JsonParser::Scanner::checkPrevious(char chr) const
{
	return peekPrevious() == chr;
}

bool JsonParser::Scanner::isAtEnd() const
{
	return m_current >= m_string.length();
}

bool JsonParser::Scanner::isValidUnescapedAsciiChar(char chr)
{
	// unescaped = %x20-21 / %x23-5B / %x5D-10FFFF
	// '"' == 0x22
	// '\' == 0x5C

	return (chr >= 0x20 && chr <= 0x21)
		|| (chr >= 0x23 && chr <= 0x5B)
		|| (chr >= 0x5D && chr <= 0x7F);
}

void JsonParser::Scanner::error(const char* message) const
{
	throw ParsingError(message, m_start);
}

void JsonParser::Scanner::addToken(TokenType type)
{
	m_tokens.push_back(Token(type, m_start));
}

void JsonParser::Scanner::addToken(TokenType type, const std::string& string)
{
	m_tokens.push_back(Token(type, string, m_start));
}

void JsonParser::Scanner::addToken(TokenType type, std::string&& string)
{
	m_tokens.push_back(Token(type, std::move(string), m_start));
}

JsonParser::Token::Token(TokenType type, const std::string& string, size_t index)
	: type(type)
	, value(string)
	, index(index)
{}

JsonParser::Token::Token(TokenType type, std::string&& string, size_t index) noexcept
	: type(type)
	, value(std::move(string))
	, index(index)
{}

JsonParser::Token::Token(TokenType type, size_t index)
	: type(type)
	, value()
	, index(index)
{}

std::string JsonParser::Token::toString() const
{
	switch (type)
	{
	case TokenType::LeftParen:    return "{";
	case TokenType::RightParen:   return "}";
	case TokenType::LeftBracket:  return "[";
	case TokenType::RightBracket: return "]";
	case TokenType::Colon:        return ":";
	case TokenType::Comma:		  return ",";
	case TokenType::True:		  return "true";
	case TokenType::False:		  return "false";
	case TokenType::Null:		  return "null";
	case TokenType::String:		  return '"' + value + '"'; 
	case TokenType::Number:		  return "num: " + value; 
	default:					  return "unkown";
	}
}

JsonParser::ParsingError::ParsingError(const char* what, size_t charIndex)
	: std::runtime_error(what)
	, m_charIndex(charIndex)
{}

size_t JsonParser::ParsingError::charIndex() const
{
	return m_charIndex;
}

Json JsonParser::parse(const std::string& string)
{
	Json object;
	JsonParser parser(string, object);
	parser.parse();
	return object;
}

JsonParser::JsonParser(const std::string& string, Json& object)
	: m_current(0)
	, m_string(string)
	, m_object(object)
	, m_nestingDepth(0)
{}

void JsonParser::parse()
{
	m_current = 0;

	Scanner scanner(m_string, m_tokens);
	scanner.scanTokens();

	m_object = value();

	// Unconsumed tokens left
	if (isAtEnd() == false)
		error("Unexpected token");
}

Json JsonParser::object()
{
	m_nestingDepth++;
	if (m_nestingDepth > MAX_NESTING_DEPTH)
		error("Max nesting depth exceeded");

	consume(TokenType::LeftParen, "Expected '{' at start of object");

	Json object = Json::emptyObject();

	if (match(TokenType::RightParen))
		return object;

	const Token& firstKey = consume(TokenType::String, "Keys must be strings");
	consume(TokenType::Colon, "Expected ':' after key");
	object[firstKey.value] = value();

	while (match(TokenType::RightParen) == false)
	{
		if (match(TokenType::Comma) == false)
			error("Key value pairs must be separated with commas");

		const Token& key = consume(TokenType::String, "Keys must be strings");
		consume(TokenType::Colon, "Expected ':' after key");
		object[key.value] = value();
	}

	m_nestingDepth--;

	return object;

}

Json JsonParser::array()
{
	m_nestingDepth++;
	if (m_nestingDepth > MAX_NESTING_DEPTH)
		error("Max nesting depth exceeded");

	consume(TokenType::LeftBracket, "Expected '[' at start of array");

	Json array = Json::emptyArray();

	if (match(TokenType::RightBracket))
		return array;

	array.array().push_back(value());
	while (match(TokenType::RightBracket) == false)
	{
		if (match(TokenType::Comma) == false)
			error("Values must be separated with commas");

		array.array().push_back(value());
	}

	m_nestingDepth--;

	return array;
}

Json JsonParser::value()
{
	switch (peek().type)
	{
	case TokenType::LeftParen: 
		return object();
	case TokenType::LeftBracket: 
		return array();
	case TokenType::True:		  
		advance();
		return Json(true);
	case TokenType::False:		  
		advance();
		return Json(false);
	case TokenType::Null:	
		advance();
		return Json(nullptr);
	case TokenType::String:		  
		advance();
		return Json(peekPrevious().value);
	case TokenType::Number:		  
		advance();
		try
		{
			return Json(std::stod(peekPrevious().value));
		}
		catch (const std::out_of_range&)
		{
			error("Number out of range of double precison floating point numbers");
		}

	default:
		error("Unexpected token");
	}
}

void JsonParser::advance()
{
	m_current++;
}

const JsonParser::Token& JsonParser::consume(TokenType token, const char* errorMessage)
{
	if (check(token))
	{
		advance();
		return peekPrevious();
	}
	else
	{
		error(errorMessage);
	}
}

bool JsonParser::check(TokenType token) const
{
	return peek().type == token;
}

bool JsonParser::match(TokenType token)
{
	if (peek().type == token)
	{
		advance();
		return true;
	}
	return false;
}

const JsonParser::Token& JsonParser::peek() const
{
	if (isAtEnd())
		error("Unexpected EOF");
	else
		return m_tokens[m_current];
}

const JsonParser::Token& JsonParser::peekPrevious() const
{
	if ((m_current - 1) >= m_tokens.size())
		error("Unexpected EOF");
	else
		return m_tokens[m_current - 1];
}

bool JsonParser::isAtEnd() const
{
	return m_current >= m_tokens.size();
}

void JsonParser::error(const char* message) const
{
	if (isAtEnd())
		throw ParsingError(message, m_string.length());
	else
		throw ParsingError(message, m_tokens[m_current].index);
}