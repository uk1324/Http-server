#pragma once

#include <iostream>
#include <string>
#include <vector>
#include <thread>
#include <fstream>
#include <filesystem>

#include "Parsing/HttpRequestParser.h"
#include "Sockets/TcpSocket.h"

class HttpServer
{
public:
	HttpServer() = default;

	void listen(uint16_t port);
	void handleClient(TcpSocket& client);
	void handleRequest(TcpSocket& client, const HttpRequest& request);
	void handleInvalidRequest(TcpSocket& client, const HttpRequest& request, HttpRequestParser::Interrupt error);
	void sendFile(int socket, const char* path, int fileSize);

private:	
	TcpSocket m_socket;
};
