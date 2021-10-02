#include "Socket.h"

int Socket::create(int protocolFamily, int type, int protocol)
{
	return socket(protocolFamily, type, protocol);
}

int Socket::close(int socketDescriptor)
{
#ifdef _WIN32
	return closesocket(socketDescriptor);
#else
	return ::close(m_handle);
#endif
}

int Socket::bind(int socketDescriptor, const sockaddr* address, int addressLength)
{
	return ::bind(socketDescriptor, address, addressLength);
}

int Socket::listen(int socketDescriptor, int maxPendingConnections)
{
	return ::listen(socketDescriptor, maxPendingConnections);
}

int Socket::accept(int socketDescriptor, sockaddr* address, int* addressLength)
{
	return ::accept(socketDescriptor, address, addressLength);
}

int Socket::send(int socketDescriptor, char* buffer, int messageLength, int flags)
{
	return ::send(socketDescriptor, buffer, messageLength, flags);
}

int Socket::recieve(int socketDescriptor, char* buffer, int bufferSize, int flags)
{
	return ::recv(socketDescriptor, buffer, bufferSize, flags);
}

const char* Socket::addressInfoErrorToString(int errorCode)
{
#ifdef _WIN32
	return gai_strerrorA(errorCode);
#else
	return gai_strerror(errorCode);
#endif

}

int Socket::getErrorCode()
{
#ifdef _WIN32
	return WSAGetLastError();
#else
	return errno();
#endif
}
