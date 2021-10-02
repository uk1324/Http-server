#pragma once

#include <array>

#include "Json.h"

std::ostream& operator<< (std::ostream& os, const Json& json);

// The implementation methods should just be in an private namespace
class JsonPrinter
{
public:
	static std::ostream& prettyPrint(std::ostream& os, const Json& json);
	static std::ostream& print(std::ostream& os, const Json& json);
	static std::string escape(const std::string& string);
private:
	static void prettyPrintImplementation(std::ostream& os, const Json& json, int depth = 1);
	static void printImplementation(std::ostream& os, const Json& json);
};

