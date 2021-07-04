#pragma once

#include <iostream>

#ifdef _WIN32
	#include <winsock2.h>
	#include <ws2tcpip.h>
	#pragma comment(lib, "Ws2_32.lib")
#else
	#include <unistd.h>
	#include <sys/socket.h>
#endif

class SocketInitializer
{
public:
	SocketInitializer()
	{
	#ifdef _WIN32
		WSADATA wsaData;
		int statusCode = WSAStartup(MAKEWORD(2, 2), &wsaData);
		if (statusCode != 0)
		{
			std::cout << "WSAStartup() failed: " << statusCode << '\n';
			std::exit(EXIT_FAILURE);
		}
	#endif
	}

	~SocketInitializer()
	{
	#ifdef _WIN32
		int statusCode = WSACleanup();
		if (statusCode != 0)
		{
			std::cout << "WSACleanup() failed" << statusCode << '\n';
		}
	#endif
	}
};

static SocketInitializer socketInitializer;