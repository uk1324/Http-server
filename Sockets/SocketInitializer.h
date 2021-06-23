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
		int result = WSAStartup(MAKEWORD(2, 2), &wsaData);
		if (result != 0)
		{
			std::cout << "WSAStarup failed: " << result << "\n";
			std::exit(-1);
		}
	#endif
	}

	~SocketInitializer()
	{
	#ifdef _WIN32
		WSACleanup();
	#endif
	}
};

static SocketInitializer socketInitializer;