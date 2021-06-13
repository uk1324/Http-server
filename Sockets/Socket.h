#pragma once

// remove socketIncluded.h later

#include <stdexcept>
#include <string>

#ifdef _WIN32
	#include <winsock2.h>
	#include <ws2tcpip.h>
	#pragma comment(lib, "Ws2_32.lib")
#else
	#include <unistd.h>
	#include <sys/socket.h>
#endif

namespace Socket
{
	int create(int protocolFamily, int type, int protocol);
	void close(int socketDescriptor);
	void bind(int socketDescriptor, const sockaddr* address, int addressLength);
	void listen(int socketDescriptor, int maxPendingConnections);

	int getErrorCode();

	static constexpr int invalidSocketDescriptor = INVALID_SOCKET;
}

