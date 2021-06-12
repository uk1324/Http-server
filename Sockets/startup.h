#pragma once

#include <iostream>
#include <cstdlib>

#include "socketIncludes.h"

inline void initializeSockets()
{
#ifdef _WIN32
	WSADATA wsaData;
	int result = WSAStartup(MAKEWORD(2, 2), &wsaData);
	if (result != 0)
	{
		std::cerr << "WSAStarup failed: " << result << "\n";
		std::exit(-1);
	}
#endif
}

inline void cleanupSockets()
{
#ifdef _WIN32
	WSACleanup();
#endif
}