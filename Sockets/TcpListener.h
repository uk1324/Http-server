#pragma once

#include <thread>

#include "SocketBase.h"
#include "TcpSocket.h"

#include <memory>

class TcpListener : public SocketBase
{
public:
	void listen(uint16_t port);
	TcpSocket accept();

private:
	void setupServer(uint16_t port);
};