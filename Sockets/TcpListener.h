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
	std::shared_ptr<TcpSocket> acceptPtr();
	int acceptDescriptor();

	template <typename Function, typename ...Args>
	std::thread acceptAndPassToThread(Function&& function, Args&&... args);

private:
	void setupServer(uint16_t port);
};

template<typename Function, typename ...Args>
std::thread TcpListener::acceptAndPassToThread(Function&& function, Args&&... args)
{
	int socketDescriptor = Socket::accept(m_socketDescriptor, nullptr, nullptr);
	if (socketDescriptor == Socket::INVALID_DESCRIPTOR)
	{
		throw SocketError("Socket::accept() error " + std::to_string(Socket::getErrorCode()));
	}

	std::tuple arguments = std::make_tuple(std::forward<Args>(args)...);

	std::thread clientThread([socketDescriptor, function, arguments] {
		TcpSocket client = TcpSocket(socketDescriptor);

		auto callArguments = std::tuple_cat(std::make_tuple(client), arguments);
		//std::invoke(function, socketDescriptor, args...);
		//std::apply()
		//function(callArguments);
		std::apply(function, callArguments);

		//function(client); 
		//function(client, args...);
	});

	//std::thread clientThread()

	return clientThread;
}
