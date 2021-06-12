#pragma once

#include <cstdint>
#include <stdexcept>
#include <string>

#include "socketIncludes.h"

class TcpSocket
{
public:
	TcpSocket() = default;

	void start(uint16_t port);
	void listen();
	TcpSocket accept();

	void send(char* buffer, int messageLength);
	void receive(char* buffer, int bufferSize, int* bytesReceived);

	void close();

	bool isValid();

private:
	TcpSocket(int socketHandle);

private:
	int m_handle;
};

