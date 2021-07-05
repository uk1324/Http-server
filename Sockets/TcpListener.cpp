#include "TcpListener.h"

void TcpListener::listen(uint16_t port)
{
	static constexpr int MAX_PENDING_CONNECTIONS = 16;

	setupServer(port);
	if (Socket::listen(m_socketDescriptor, MAX_PENDING_CONNECTIONS) == Socket::SOCKET_ERR)
	{
		throw SocketError("Socket::listen() error: " + std::to_string(Socket::getErrorCode()));
	}
}

TcpSocket TcpListener::accept()
{
	int socketDescriptor = Socket::accept(m_socketDescriptor, nullptr, nullptr);
	if (socketDescriptor == Socket::INVALID_DESCRIPTOR)
	{
		throw SocketError("Socket::accept() error " + std::to_string(Socket::getErrorCode()));
	}
	return TcpSocket(socketDescriptor);
}

std::shared_ptr<TcpSocket> TcpListener::acceptPtr()
{
	int socketDescriptor = Socket::accept(m_socketDescriptor, nullptr, nullptr);
	if (socketDescriptor == Socket::INVALID_DESCRIPTOR)
	{
		throw SocketError("Socket::accept() error " + std::to_string(Socket::getErrorCode()));
	}
	return std::shared_ptr<TcpSocket>(new TcpSocket(socketDescriptor));
	//return /*std::make_shared<TcpSocket>(TcpSocket(socketDescriptor));*/
}

int TcpListener::acceptDescriptor()
{
	int socketDescriptor = Socket::accept(m_socketDescriptor, nullptr, nullptr);
	if (socketDescriptor == Socket::INVALID_DESCRIPTOR)
	{
		throw SocketError("Socket::accept() error " + std::to_string(Socket::getErrorCode()));
	}
	return socketDescriptor;
}

void TcpListener::setupServer(uint16_t port)
{
	struct addrinfo hints { 0 };
	struct addrinfo* servinfo;

	// IPv4 address
	hints.ai_family = AF_INET;
	// TCP protocol
	hints.ai_socktype = SOCK_STREAM;

	std::string portString = std::to_string(static_cast<unsigned int>(port));
	int status = getaddrinfo(nullptr, portString.c_str(), &hints, &servinfo);
	if (status != 0)
	{
		throw SocketError("Socket::getaddrinfo() error: " + std::to_string(status));
	}

	m_socketDescriptor = Socket::create(servinfo->ai_family, servinfo->ai_socktype, IPPROTO_TCP);
	if (m_socketDescriptor == Socket::INVALID_DESCRIPTOR)
	{
		freeaddrinfo(servinfo);
		throw SocketError("Socket::create() error: " + std::to_string(Socket::getErrorCode()));
	}

	if (Socket::bind(m_socketDescriptor, servinfo->ai_addr, servinfo->ai_addrlen) == Socket::SOCKET_ERR)
	{
		freeaddrinfo(servinfo);
		throw SocketError("Socket::bind() error:" + std::to_string(Socket::getErrorCode()));
	}
}
