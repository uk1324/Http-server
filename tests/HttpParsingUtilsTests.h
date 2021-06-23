#pragma once

#include "TestingUtils.h"
#include "../HttpParsingUtils.h"

namespace Tests
{
	class HttpParsingUtilsTests : public TestCase
	{
	public:
		using TestCase::TestCase;

		void tryDecodePercentEncodedStringTest1()
		{
			std::string string = "%54%65%53t%20Ca%73e%0d%0A";
			std::string result = HttpParsingUtils::tryDecodePercentEncodedString(string);
			assertEquals<std::string>(result, "TeSt Case\r\n");
		}

		void tryDecodePercentEncodedStringTest2()
		{
			std::string string = "%5%00abc%a";
			bool parsingFailed = false;
			try
			{
				std::string result = HttpParsingUtils::tryDecodePercentEncodedString(string);
			}
			catch (std::exception&)
			{
				parsingFailed = true;
			}
			assertTrue(parsingFailed, "Parsing doesn't fail with invalid input");
		}

		void runAllTests()
		{
			displayRunningMessage();

			tryDecodePercentEncodedStringTest1();
			tryDecodePercentEncodedStringTest2();

			displayTestsPassed();
		}
	};
}