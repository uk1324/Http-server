#pragma once

#include <cstdint>
#include <stdexcept>
#include <string>

#include "SocketBase.h"

class TcpListener;

class TcpSocket : public SocketBase
{
	friend TcpListener;

public:
	static TcpSocket fromDescriptor(int socketDescriptor);
	void send(char* buffer, int messageLength);
	void receive(char* buffer, int bufferSize, int* bytesReceived);

private:
	explicit TcpSocket(int socketDescriptor);
};

