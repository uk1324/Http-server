#include "HttpServer.h"
#include "CaseInsensitiveLookupTable.h"
#include "./Sockets/startup.h"

#include "CaseInsensitiveString.h"

int main()
{

	//HttpRequestHeader header;
 	//header.parse("GET /favicon.ico HTTP/1.1\r\nHost: localhost:8080\r\nUser-Agent : Mozilla / 5.0 (Windows NT 10.0; Win64; x64; rv:87.0) Gecko / 20100101 Firefox / 87.0\r\nAccept : image / webp, */*\r\nAccept-Language: pl,en-US;q=0.7,en;q=0.3\r\nAccept-Encoding: gzip, deflate\r\nConnection: keep-alive\r\nReferer: http://localhost:8080/\r\nCache-Control: max-age=0\r\ncure-Requests: 1\r\nCache-Control: max-age=0\r\n\r\n", 382u);

	//namespace fs = std::filesystem;

	//fs::path p("../index.html");
	//std::cout << p << '\n';
	//std::cout << p.parent_path() << '\n';
	//std::cout << fs::current_path() << '\n';
	//std::cout << fs::exists(p) << '\n';

	//std::cout << sizeof fs::path << '\n';

	//HttpResponseHeader header;
	//header.m_headers["Content-type"] = "text/html";
	//header.m_headers["Content-length"] = "200";

	//std::cout << header.toString() << '\n';

	//const char buffer[] = "abcdef     adfasdf abc";

	//const char buffer[] = "GET / HTTP/1.1\r\nHello: there\r\n\r\n";

	//HttpRequestParser parser(buffer, sizeof(buffer) - 1);
	//parser.parse();

	//CaseInsensitiveMap<std::string> a;

	//a["Hello"] = "you";
	//std::cout << a["heLlO"] << '\n';

	//CaseInsensitiveString a = "Hello";
	//CaseInsensitiveString b = "heLLO";

	//std::cout << (a == b) << '\n';

	initializeSockets();

	HttpServer server;
	try
	{
		server.listen(8080);
	}
	catch (std::exception& err)
	{
		std::cout << err.what() << '\n';
	}

	cleanupSockets();
}