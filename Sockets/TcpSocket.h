#pragma once

#include <cstdint>
#include <stdexcept>
#include <string>

#include "Socket.h"

class TcpSocket
{
public:
	TcpSocket() = default;
	~TcpSocket();

	TcpSocket(const TcpSocket&) = delete;
	TcpSocket& operator=(TcpSocket const&) = delete;

	void listen(uint16_t port);
	TcpSocket accept();

	void send(char* buffer, int messageLength);
	void receive(char* buffer, int bufferSize, int* bytesReceived);

	void close();

	bool isValid();

private:
	void setupServer(uint16_t port);

	TcpSocket(int socketDescriptor);

private:
	int m_socketDescriptor = Socket::invalidSocketDescriptor;
};

