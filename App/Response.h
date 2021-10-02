#pragma once

#include "../Http/Messages/HttpResponse.h"
#include "../Sockets/TcpSocket.h"

class Response : public HttpResponse
{
public:
	Response(TcpSocket& socket);

	void send(const std::string& string);
	void end();

	TcpSocket& socket();
	bool isFinished();
private:
	TcpSocket& m_socket;
	bool m_finished;
};