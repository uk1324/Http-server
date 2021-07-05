#pragma once

#include "Socket.h"
#include "SocketError.h"

class SocketBase
{
public:
	SocketBase() = default;
	SocketBase(const SocketBase&) = delete;
	SocketBase operator= (const SocketBase&) = delete;
	SocketBase(SocketBase&& other) noexcept;
	SocketBase& operator=(SocketBase&& othexr) noexcept;
	~SocketBase();

public:
	void close();

protected:
	int m_socketDescriptor;
};

