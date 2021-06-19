#include "TcpSocket.h"

#include <iostream>

TcpSocket::~TcpSocket()
{
	//close();
}


void TcpSocket::listen(uint16_t port)
{
	static constexpr int maxPendingConnections = 10;

	try
	{
		setupServer(port);
		Socket::listen(m_socketDescriptor, maxPendingConnections);
	}
	catch (std::runtime_error& error)
	{
		throw error;
	}
}

TcpSocket TcpSocket::accept()
{
	return TcpSocket(::accept(m_socketDescriptor, nullptr, nullptr));
}


void TcpSocket::send(char* buffer, int messageLength)
{
	::send(m_socketDescriptor, buffer, messageLength, 0);
}

void TcpSocket::receive(char* buffer, int bufferSize, int* bytesReceived)
{
	*bytesReceived = recv(m_socketDescriptor, buffer, bufferSize, 0);
}

void TcpSocket::close()
{
	std::cout << "closed " << m_socketDescriptor << '\n';
	if (m_socketDescriptor != Socket::invalidSocketDescriptor)
	{
		try
		{
			Socket::close(m_socketDescriptor);
		}
		catch (std::runtime_error& error)
		{
			throw error;
		}

		m_socketDescriptor = Socket::invalidSocketDescriptor;
	}
}

bool TcpSocket::isValid()
{
	return m_socketDescriptor != Socket::invalidSocketDescriptor;
}

void TcpSocket::setupServer(uint16_t port)
{
	struct addrinfo hints { 0 };
	struct addrinfo* servinfo;

	// ADDR_ANY migth be better here
	// IPv4 address
	hints.ai_family = AF_INET;
	// TCP protocol
	hints.ai_socktype = SOCK_STREAM;
	// 
	hints.ai_flags = AI_PASSIVE;

	int status;

	char portStr[6];
	sprintf(portStr, "%u", port);

	if ((status = getaddrinfo(nullptr, portStr, &hints, &servinfo)) != 0)
	{
	#ifdef _WIN32
		throw std::runtime_error("getaddrinfo() error: " + std::string(gai_strerrorA(status)) + '\n');
	#else
		throw std::runtime_error("getaddrinfo() error: " + std::string(gai_strerror(status)));
	#endif
	}

	try
	{
		m_socketDescriptor = Socket::create(servinfo->ai_family, servinfo->ai_socktype, IPPROTO_TCP);
		Socket::bind(m_socketDescriptor, servinfo->ai_addr, servinfo->ai_addrlen);
	}
	catch (std::runtime_error& error)
	{
		throw error;
	}

	freeaddrinfo(servinfo);
}

TcpSocket::TcpSocket(int socketDescriptor)
	: m_socketDescriptor(socketDescriptor)
{
	if (m_socketDescriptor != -1)
	{
		std::cout << "created " << m_socketDescriptor << '\n';
	}
}