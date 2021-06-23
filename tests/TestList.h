#pragma once

#include <vector>
#include <memory>

#include "HttpParsingUtilsTests.h"
#include "HttpRequestParserTests.h"

namespace Tests
{
//#define RUN_ALL_TESTS

#ifdef RUN_ALL_TESTS

	int runAllTests()
	{
		std::vector<std::shared_ptr<TestCase>> tests = {
			std::make_shared<HttpParsingUtilsTests>("HttpParsingUtilsTests")
		};

		for (auto& test : tests)
		{
			test->runAllTests();
		}
		return 0;
	}

	int testsRun = runAllTests();
#endif
}