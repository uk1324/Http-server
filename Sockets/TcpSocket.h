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
	void send(const char* buffer, int messageLength);
	void receive(char* buffer, int bufferSize, int* bytesReceived);

//private:
public:
	explicit TcpSocket(int socketDescriptor);
};

