#pragma once

#include <stdexcept>
#include <string>

#include "SocketInitializer.h"

namespace Socket
{
	int create(int protocolFamily, int type, int protocol);
	void close(int socketDescriptor);
	void bind(int socketDescriptor, const sockaddr* address, int addressLength);
	void listen(int socketDescriptor, int maxPendingConnections);

	int getErrorCode();

	static constexpr int invalidSocketDescriptor = INVALID_SOCKET;
}

