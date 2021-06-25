#include "TestingUtils.h"

Tests::TestCase::TestCase(const std::string& name)
	: name(name)
{}

void Tests::TestCase::assertTrue(bool test, std::string message)
{
	testCount++;

	if (test == true)
	{
		testsPassed++;
	}
	else
	{
		std::cout << "Assert failed: " << message << '\n';
	}
}

void Tests::TestCase::assertFalse(bool test, std::string message)
{
	testCount++;

	if (test == false)
	{
		testsPassed++;
	}
	else
	{
		std::cout << "Assert failed: " << message << '\n';
	}
}

void Tests::TestCase::displayRunningMessage()
{
	std::cout << "Running " << name << '\n';
}

void Tests::TestCase::displayTestsPassed()
{
	std::cout << "Passed " << testsPassed << " out of " << testCount << " tests.\n";
}
