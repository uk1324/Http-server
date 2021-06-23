#include "Socket.h"

int Socket::create(int protocolFamily, int type, int protocol)
{
	int socketDescriptor = socket(protocolFamily, type, protocol);
	if (socketDescriptor == invalidSocketDescriptor)
		throw std::runtime_error("Socket::create() error " + std::to_string(getErrorCode()) + '\n');
	return socketDescriptor;
}

void Socket::close(int socketDescriptor)
{
#ifdef _WIN32
	int status = closesocket(socketDescriptor);
#else
	int status = ::close(m_handle);
#endif

	if (status == SOCKET_ERROR)
		throw std::runtime_error("Socket::close() error " + std::to_string(getErrorCode()) + '\n');
}

void Socket::bind(int socketDescriptor, const sockaddr* address, int addressLength)
{
	if (::bind(socketDescriptor, address, addressLength) == SOCKET_ERROR)
		throw std::runtime_error("Socket::bind() error " + std::to_string(getErrorCode()) + '\n');
}

void Socket::listen(int socketDescriptor, int maxPendingConnections)
{
	if (::listen(socketDescriptor, maxPendingConnections) == SOCKET_ERROR)
		throw std::runtime_error("Socket::listen error" + std::to_string(getErrorCode()) + '\n');
}

int Socket::getErrorCode()
{
#ifdef _WIN32
	return WSAGetLastError();
#else
	return errno();
#endif
}
