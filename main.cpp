#include "HttpServer.h"

#include "tests/TestList.h"

#include "HttpHeaders.h"

int main()
{
	//Tests::HttpParsingUtilsTests a;
	//a.runAllTests();

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