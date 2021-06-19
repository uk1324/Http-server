#pragma once

#include "utils.h"
#include "../Parsing/HttpRequestParser.h"

namespace Tests
{
	class HttpRequestParserTests
	{
		static void methodParsingTest1()
		{
			bool finishedParsing = false, parsedProperly; 
			char buffer[] = "GET / HTTP/1.1\r\n\r\n";

			HttpRequestParser parser(buffer);
			try
			{
				parser.parse(sizeof(buffer) - 1);
			}
			catch (HttpRequestParser::Interrupt interrupt)
			{
				finishedParsing = (interrupt == HttpRequestParser::Interrupt::finishedParsingFullRequest);
			}

			parsedProperly = parser.result().method == HttpMethod::Get;

			assertTrue(finishedParsing && parsedProperly, "Basic method parsing");
		}

		static void methodParsingTest2()
		{
			bool finishedParsing = false, parsedProperly;
			char buffer[] = "POST / HTTP/1.1\r\n\r\n";

			HttpRequestParser parser(buffer);
			try
			{
				parser.parse(sizeof(buffer) - 1);
			}
			catch (HttpRequestParser::Interrupt interrupt)
			{
				finishedParsing = (interrupt == HttpRequestParser::Interrupt::finishedParsingFullRequest);
			}

			parsedProperly = parser.result().method == HttpMethod::Post;

			assertTrue(finishedParsing && parsedProperly, "Basic method parsing2");
		}

	public:
		static void runAllTests()
		{
			// doesn't break when it method starts parsing when reachBufferEnd
			methodParsingTest1();
			methodParsingTest2();
		}
	};
}