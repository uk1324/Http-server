#pragma once

#include "Response.h"

Response::Response(TcpSocket& socket)
	: m_socket(socket)
	, m_finished(false)
{}

void Response::send(const std::string& string)
{
	this->headers["Content-Length"] = std::to_string(string.length());
	std::string response = this->toString();
	m_socket.send(response.data(), response.length());
	m_socket.send(string.data(), string.length());
	end();
}

void Response::end()
{
	m_finished = true;
}

TcpSocket& Response::socket()
{
	return m_socket;
}

bool Response::isFinished()
{
	return m_finished;
}
