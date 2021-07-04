#pragma once

#include "Socket.h"
#include "SocketError.h"

class SocketBase
{
public:
	SocketBase() = default;
	~SocketBase();
	SocketBase(const SocketBase&) = delete;
	SocketBase operator= (const SocketBase&) = delete;

public:
	void close();

protected:
	int m_socketDescriptor;
};

