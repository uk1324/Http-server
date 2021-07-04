#pragma once

#include "SocketBase.h"
#include "TcpSocket.h"

class TcpListener : public SocketBase
{
public:
	void listen(uint16_t port);
	TcpSocket accept();
	int acceptDescriptor();

	template <typename Function, typename ...Args>
	void acceptAndPassToThread(Function&& function, Args&&... args);

private:
	void setupServer(uint16_t port);
};

template<typename Function, typename ...Args>
void TcpListener::acceptAndPassToThread(Function&& function, Args && ...args)
{
	int socketDescriptor = Socket::accept(m_socketDescriptor, nullptr, nullptr);
	if (socketDescriptor == Socket::INVALID_DESCRIPTOR)
	{
		throw SocketError("Socket::accept() error " + std::to_string(Socket::getErrorCode()));
	}
	
	std::thread clientThread([this, clientDescriptor] {
		TcpSocket client = TcpSocket::fromDescriptor(clientDescriptor);
		this->handleClient(client);
	});

}
