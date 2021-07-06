#include "HttpParsingUtils.h"

int HttpParsingUtils::hexDigitToInt(char digit)
{
	if ((digit >= 'A') && (digit <= 'F'))
	{
		return digit - 'A' + 10;
	}
	else if ((digit >= 'a') && (digit <= 'f'))
	{
		return digit - 'a' + 10;
	}
	else if ((digit >= '0') && (digit <= '9'))
	{
		return digit - '0';
	}
	else
	{
		return -1;
	}
	
}

std::string HttpParsingUtils::tryDecodePercentEncodedString(const std::string& string)
{
	std::string result;
	size_t i = 0;
	while (i < string.length())
	{
		if (string[i] == '%')
		{
			static constexpr size_t hexOctetStringLength = 2;
			if (i + hexOctetStringLength == string.length())
				throw std::exception();

			if ((isxdigit(string[i + 1]) == false) || (isxdigit(string[i + 2]) == false))
				throw std::exception();

			char value = hexDigitToInt(string[i + 1]) * 0x10 + hexDigitToInt(string[i + 2]);
			result += value;

			i += 3;
		}
		else
		{
			result += string[i];
			i++;
		}
	}
	return result;
}

bool HttpParsingUtils::isUnreservedUriChar(char chr)
{
	// https://datatracker.ietf.org/doc/html/rfc3986#appendix-A
	// unreserved = ALPHA / DIGIT / "-" / "." / "_" / "~"
	return std::isalnum(chr)
		|| chr == '-'
		|| chr == '.'
		|| chr == '_'
		|| chr == '~';
}

bool HttpParsingUtils::isSubDelimeterChar(char chr)
{
	// https://datatracker.ietf.org/doc/html/rfc3986#appendix-A
	// sub-delims = "!" / "$" / "&" / "'" / "(" / ")" / "*" / "+" / "," / ";" / "="
	return chr == '!'
		|| chr == '$'
		|| chr == '&'
		|| chr == '\''
		|| chr == '/'
		|| chr == '*'
		|| chr == '+'
		|| chr == ','
		|| chr == ';'
		|| chr == '=';
}

bool HttpParsingUtils::isPchar(char chr)
{
	// https://datatracker.ietf.org/doc/html/rfc3986#appendix-A
	// pchar = unreserved / pct-encoded / sub-delims / ":" / "@"
	return isUnreservedUriChar(chr)
		|| chr == '%'
		|| isSubDelimeterChar(chr)
		|| chr == ':'
		|| chr == '@';
}

bool HttpParsingUtils::isTokenChar(char chr)
{
	// https://datatracker.ietf.org/doc/html/rfc7230#section-3.2.6
	// tchar = "!" / "#" / "$" / "%" / "&" / "'" / "*" / "+" / "-" / "." / "^" / "_" / "`" / "|" / "~" / DIGIT / ALPHA
	// token = 1*tchar
	return chr == '!'
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
		|| chr == '~'
		|| isalnum(chr);
}

bool HttpParsingUtils::isVisiblechar(char chr)
{
	// https://datatracker.ietf.org/doc/html/rfc7230#section-1.2
	// https://datatracker.ietf.org/doc/html/rfc5234#appendix-B.1
	// VCHAR = % x21-7E ; visible(printing) characters
	return chr >= 0x21 && chr <= 0x7E;
}

bool HttpParsingUtils::isObsChar(char chr)
{
	// https://datatracker.ietf.org/doc/html/rfc7230#section-3.2.6
	// obs-text = %x80-FF
	return chr >= 0x80 && chr <= 0xFF;
}

bool HttpParsingUtils::isWhitespace(char chr)
{
	return chr == ' ' || chr == '\t';
}

bool HttpParsingUtils::isValidHeaderNameChar(char chr)
{
	// https://datatracker.ietf.org/doc/html/rfc7230#section-3.2
	// field-name = token
	return isTokenChar(chr);
}

bool HttpParsingUtils::isValidHeaderValueChar(char chr)
{
	// https://datatracker.ietf.org/doc/html/rfc7230#section-3.2
	// field-value = *(field-content / obs - fold)
	// field-content = field-vchar[1 * (SP / HTAB) field-vchar]
	// field-vchar = VCHAR / obs-text
	return isVisiblechar(chr)
		|| isObsChar(chr)
		|| chr == ' '
		|| chr == '\t';
}