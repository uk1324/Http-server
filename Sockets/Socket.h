#pragma once

#include <stdexcept>
#include <string>

#include "SocketInitializer.h"

// This namespace isn't pretty much useless
namespace Socket
{
	int create(int protocolFamily, int type, int protocol);
	int close(int socketDescriptor);
	int bind(int socketDescriptor, const sockaddr* address, int addressLength);
	int listen(int socketDescriptor, int maxPendingConnections);
	int accept(int socketDescriptor, sockaddr* address, int* addressLength);
	int send(int socketDescriptor, char* buffer, int messageLength, int flags);
	int recieve(int socketDescriptor, char* buffer, int bufferSize, int flags);

	const char* addressInfoErrorToString(int errorCode);

	int getErrorCode();

	static constexpr int INVALID_DESCRIPTOR = INVALID_SOCKET;
	static constexpr int SOCKET_ERR = SOCKET_ERROR;
}

