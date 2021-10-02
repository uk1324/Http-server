#include "JsonPrinter.h"

std::ostream& JsonPrinter::prettyPrint(std::ostream& os, const Json& json)
{
	prettyPrintImplementation(os, json);
	return os;
}

std::ostream& JsonPrinter::print(std::ostream& os, const Json& json)
{
	printImplementation(os, json);
	return os;
}

void JsonPrinter::prettyPrintImplementation(std::ostream& os, const Json& json, int depth)
{
	static constexpr char tab[] = "  ";
	auto printTabs = [&os](int count) {
		for (int i = 0; i < count; i++)
			os << tab;
	};

	switch (json.type())
	{
	case Json::Type::String:
		os << '"' << escape(json.string()) << '"';
		break;

	case Json::Type::Number:
		os << json.number();
		break;

	case Json::Type::Boolean:
		os << (json.boolean() ? "true" : "false");
		break;

	case Json::Type::Null:
		os << "null";
		break;

	case Json::Type::Object:
		if (json.object().empty())
		{
			os << "{}\n";
		}
		else
		{
			// std::unordered_map doesn't overload the subtraction operator so the decrement operator is used.
			os << "{\n";
			for (auto it = json.object().cbegin(); it != (--json.object().cend()); it++)
			{
				printTabs(depth);
				os << '"' << it->first << "\": ";
				prettyPrintImplementation(os, it->second, depth + 1);
				os << ",\n";
			}
			printTabs(depth);
			os << '"' << (--json.object().cend())->first << "\": ";
			prettyPrintImplementation(os, (--json.object().cend())->second, depth + 1);
			os << "\n";

			printTabs(depth - 1);
			os << "}";
		}
		break;

	case Json::Type::Array:
		if (json.array().empty())
		{
			os << "[]\n";
		}
		else
		{
			os << "[\n";
			for (auto it = json.array().cbegin(); it != json.array().cend() - 1; it++)
			{
				printTabs(depth);
				prettyPrintImplementation(os, *it, depth + 1);
				os << ",\n";
			}
			printTabs(depth);
			prettyPrintImplementation(os, *(json.array().cend() - 1), depth + 1);
			os << "\n";

			printTabs(depth - 1);
			os << "]";
		}
		break;
	}
}

void JsonPrinter::printImplementation(std::ostream& os, const Json& json)
{
	switch (json.type())
	{
	case Json::Type::String:
		os << '"' << escape(json.string()) << '"';
		break;

	case Json::Type::Number:
		os << json.number();
		break;

	case Json::Type::Boolean:
		os << (json.boolean() ? "true" : "false");
		break;

	case Json::Type::Null:
		os << "null";
		break;

	case Json::Type::Object:
		if (json.object().empty())
		{
			os << "{}";
		}
		else
		{
			// std::unordered_map doesn't overload the subtraction operator so the decrement operator is used.
			os << '{';
			for (auto it = json.object().cbegin(); it != (--json.object().cend()); it++)
			{
				os << '"' << it->first << "\":";
				print(os, it->second);
				os << ',';
			}
			os << '"' << (--json.object().cend())->first << "\":";
			print(os, (--json.object().cend())->second);

			os << '}';
		}
		break;

	case Json::Type::Array:
		if (json.array().empty())
		{
			os << "[]";
		}
		else
		{
			os << '[';
			for (auto it = json.array().cbegin(); it != json.array().cend() - 1; it++)
			{
				print(os, *it);
				os << ',';
			}
			print(os, *(json.array().cend() - 1));
			os << ']';
		}
		break;
	}
}

std::string JsonPrinter::escape(const std::string& string)
{
	// unescaped = %x20-21 / %x23-5B / %x5D-10FFFF

	std::string escaped;
	escaped.reserve(string.length());

	for (char chr : string)
	{
		switch (chr)
		{
		case '"':
			escaped += "\\\"";
			break;
		case '\\':
			escaped += "\\\\";
			break;
		case '\b':
			escaped += "\\b";
			break;
		case '\f':
			escaped += "\\f";
			break;
		case '\n':
			escaped += "\\n";
			break;
		case '\r':
			escaped += "\\r";
			break;
		case '\t':
			escaped += "\\t";
			break;
		default:
			static constexpr char hexToInt[] = "0123456789abcdef";
			unsigned char c = chr;
			if (c < 0x10)
			{
				std::cout << int(chr) << '\n';
				escaped += "\\u000";
				escaped += hexToInt[c];
			}
			else if (c < 0x20)
			{
				escaped += "\\u001";
				escaped += hexToInt[c - 0x10];
			}
			else
			{
				escaped += c;
			}
			break;
		}
	}

	return escaped;
}

std::ostream& operator<< (std::ostream& os, const Json& json)
{
	return JsonPrinter::print(os, json);
}
