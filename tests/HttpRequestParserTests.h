#pragma once

#include "utils.h"
#include "../Parsing/HttpRequestParser.h"

namespace Tests
{
	class HttpRequestParserTests
	{
		static void methodParsingTest1()
		{
			bool test;
			const char* buffer = "GET / HTTP/1.1\r\n";

			HttpRequestParser parser(buffer);
			try
			{
				parser.parse(sizeof(buffer) - 1);
			}
			catch (HttpRequestParser::Interrupt interrupt)
			{
				//if ()
			}

			test = parser.result().method == HttpMethod::Get;
			assertTrue(test, "Basic method parsing");
		}

		static void runAllTests()
		{
			methodParsingTest1();
		}
	};
}