#pragma once
class LinearParser
{
protected:
	static int hashChar(int lastHash, char chr);
	static int hashString(const char* string);
};

