#pragma once

#ifdef _WIN32
	#include <winsock2.h>
	#include <ws2tcpip.h>

	#pragma comment(lib, "Ws2_32.lib")
#endif