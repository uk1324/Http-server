#include "TcpSocket.h"

TcpSocket::TcpSocket(int socketHandle)
	: m_handle(socketHandle)
{}

void TcpSocket::start(uint16_t port)
{
	struct addrinfo hints { 0 };
	struct addrinfo* servinfo;

	hints.ai_family = AF_INET;
	hints.ai_socktype = SOCK_STREAM;
	hints.ai_flags = AI_PASSIVE;

	int status;

	char portStr[6];
	sprintf(portStr, "%u", port);

	if ((status = getaddrinfo(nullptr, portStr, &hints, &servinfo)) != 0)
	{
#ifdef _WIN32
		throw std::runtime_error("getaddrinfo() error: " + std::string(gai_strerrorA(status)));
#else
		throw std::runtime_error("getaddrinfo() error: " + std::string(gai_strerror(status)));
#endif
	}

	m_handle = socket(servinfo->ai_family, servinfo->ai_socktype, 0);
	if (m_handle == INVALID_SOCKET)
	{
#ifdef _WIN32
		throw std::runtime_error("socket() error: " + std::to_string(WSAGetLastError()));
#else
		throw std::runtime_error("socket() error: " + std::string(errno);
#endif
	}

	if (bind(m_handle, servinfo->ai_addr, servinfo->ai_addrlen) == SOCKET_ERROR)
	{
		close();
#ifdef _WIN32
		throw std::runtime_error("bind() error: " + std::to_string(WSAGetLastError()));
#else
		throw std::runtime_error("bind() error: " + std::string(errno);
#endif
	}
	freeaddrinfo(servinfo);
}


void TcpSocket::listen()
{
	if (::listen(m_handle, 10) == SOCKET_ERROR)
	{
#ifdef _WIN32
		throw std::runtime_error("listen() error: " + std::to_string(WSAGetLastError()));
#else
		throw std::runtime_error("listen() error: " + std::string(errno);
#endif
	}
}

TcpSocket TcpSocket::accept()
{
	return TcpSocket(::accept(m_handle, NULL, NULL));
}


void TcpSocket::send(char* buffer, int messageLength)
{
	::send(m_handle, buffer, messageLength, 0);
}

void TcpSocket::receive(char* buffer, int bufferSize, int* bytesReceived)
{
	*bytesReceived = recv(m_handle, buffer, bufferSize, 0);
}

void TcpSocket::close()
{
#ifdef _WIN32
	closesocket(m_handle);
#else
	close(m_handle);
#endif
}

bool TcpSocket::isValid()
{
	return m_handle != INVALID_SOCKET;
}
