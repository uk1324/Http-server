#include "utils.h"

void Tests::assertTrue(bool test, const char* description)
{
	std::cout << ((test == true) ? "[Passed] " : "[Failed] ")
			  << description << '\n';
}
