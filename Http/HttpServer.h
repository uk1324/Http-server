#pragma once

#include <iostream>
#include <string>
#include <vector>
#include <thread>
#include <fstream>
#include <filesystem>

#include "Parsing/HttpRequestParser.h"
#include "Messages/HttpResponse.h"
#include "../Sockets/TcpListener.h"

// Maybe a better way to handle requests with a thread pool
// https://stackoverflow.com/questions/65904064/how-to-make-a-multipart-server-in-c-how-to-start-a-thread-without-waiting-fo

// Static asssert that recv buffer size is not smaller than max request line size
class HttpServer
{
public:
	HttpServer() = default;
	virtual ~HttpServer() = default;

	void listen(uint16_t port);
private:
	void handleClient(TcpSocket&& socket);
	virtual void handleRequest(TcpSocket& socket, const HttpRequest& request) = 0;
	virtual void handleInvalidRequest(TcpSocket& socket, HttpRequestParser::Error error) = 0;
private:	
	TcpListener m_socket;
};
