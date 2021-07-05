#include "SocketBase.h"

SocketBase::~SocketBase()
{
	close();
}

SocketBase& SocketBase::operator=(SocketBase&& other) noexcept
{
	close();
	m_socketDescriptor = other.m_socketDescriptor;
	other.m_socketDescriptor = Socket::INVALID_DESCRIPTOR;
	return *this;
}

SocketBase::SocketBase(SocketBase&& other) noexcept
	:m_socketDescriptor(other.m_socketDescriptor)
{
	other.m_socketDescriptor = -1;
}

void SocketBase::close()
{
	// shouldn't throw in descriptor
	if (m_socketDescriptor != Socket::INVALID_DESCRIPTOR)
	{
		if (Socket::close(m_socketDescriptor) == Socket::SOCKET_ERR)
			throw ("Socket::close() error " + std::to_string(Socket::getErrorCode()));

		m_socketDescriptor = Socket::INVALID_DESCRIPTOR;
	}
}
