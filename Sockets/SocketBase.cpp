#include "SocketBase.h"

SocketBase::~SocketBase()
{
	close();
}

void SocketBase::close()
{
	if (m_socketDescriptor != Socket::INVALID_DESCRIPTOR)
	{
		if (Socket::close(m_socketDescriptor) == SOCKET_ERROR)
			throw ("Socket::close() error " + std::to_string(Socket::getErrorCode()));

		m_socketDescriptor = Socket::INVALID_DESCRIPTOR;
	}
}
