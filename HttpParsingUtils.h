#pragma once

#include <string>
#include <stdexcept>

namespace HttpParsingUtils
{
	int hexDigitToInt(char digit);
	std::string tryDecodePercentEncodedString(const std::string& string);

	bool isUnreservedUriChar(char chr);
	bool isSubDelimeterChar(char chr);
	bool isPchar(char chr);
	bool isTokenChar(char chr);
	bool isVisiblechar(char chr);
	bool isObsChar(char chr);
	bool isWhitespace(char chr);
	bool isValidHeaderNameChar(char chr);
	bool isValidHeaderValueChar(char chr);
};

