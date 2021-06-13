#pragma once

#include <iostream>

#include "Socket.h"

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