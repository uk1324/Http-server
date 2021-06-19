#include "HttpServer.h"

#include "CaseInsensitiveString.h"

#include "Sockets/SocketInitializer.h"

#include "tests/HttpRequestParserTests.h"

int main()
{
	//using namespace Tests;
	//HttpRequestParserTests::runAllTests();


	//std::string a;

	//char b[] = { 'a', 'b', '\0', 'c', 'd' };

	//for (auto c : b)
	//{
	//	a += c;
	//	std::cout << a << " len: " << a.length() << '\n';
	//}

	//CaseInsensitiveString a = "abc";
	//CaseInsensitiveString b = std::string("abcd");

	// path could be percent encoded
	// first parse percent encoding then parse the actual string skip nulls

	HttpServer server;
	try
	{
		server.listen(8080);
	}
	catch (std::exception& err)
	{
		std::cout << err.what() << '\n';
	}
}