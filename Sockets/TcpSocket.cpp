#include "TcpSocket.h"

TcpSocket TcpSocket::fromDescriptor(int socketDescriptor)
{
	return TcpSocket(socketDescriptor);
}

void TcpSocket::send(const char* buffer, int messageLength)
{
	::send(m_socketDescriptor, buffer, messageLength, 0);
}

void TcpSocket::receive(char* buffer, int bufferSize, int* bytesReceived)
{
	*bytesReceived = recv(m_socketDescriptor, buffer, bufferSize, 0);
}

TcpSocket::TcpSocket(int socketDescriptor)
{
	m_socketDescriptor = socketDescriptor;
}