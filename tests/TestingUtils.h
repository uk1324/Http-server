#pragma once

#include <iostream>

namespace Tests
{
	class TestCase
	{
	public:
		explicit TestCase(const std::string& name);

		template <typename T>
		void assertEquals(T result, T wantedResult);
		void assertTrue(bool test, const char* message);
		void assertFalse(bool test, const char* message);

		virtual void runAllTests() = 0;

		void displayRunningMessage();
		void displayTestsPassed();
	private:
		std::string name;
		int testCount = 0;
		int testsPassed = 0;
	};
};

template<typename T>
void Tests::TestCase::assertEquals(T result, T wantedResult)
{
	testCount++;

	if (result == wantedResult)
	{
		testsPassed++;
	}
	else
	{
		std::cout << "Assert failed: " << result << " != " << wantedResult << '\n';
	}
}