#include "HttpServer.h"

#include "CaseInsensitiveString.h"

#include "Sockets/SocketInitializer.h"

int main()
{
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