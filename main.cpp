#include "HttpServer.h"

#include "tests/TestList.h"

#include "HttpHeaders.h"

int main()
{
	//Tests::HttpRequestParserTests a("HttpParsingUtilsTests");
	//a.runAllTests();

	HttpServer server;
	try
	{
		server.listen(8080);
	}
	catch (std::exception& err)
	{
		throw;
		std::cout << err.what() << '\n';
	}
}