#pragma once

#include <iostream>
#include <string>
#include <vector>
#include <thread>
#include <fstream>
#include <filesystem>

#include "Parsing/HttpRequestParser.h"
#include "HttpResponse.h"
#include "Sockets/TcpListener.h"

class HttpServer
{
public:
	HttpServer() = default;

	void listen(uint16_t port);
	void handleClient(TcpSocket& socket);
	void handleRequest(TcpSocket& socket, const HttpRequest& request);
	void handleInvalidRequest(TcpSocket& socket, const HttpRequest& request, HttpRequestParser::Interrupt error);

private:	
	TcpListener m_socket;
};
