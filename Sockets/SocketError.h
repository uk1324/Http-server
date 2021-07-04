#pragma once

#include <stdexcept>

class SocketError : public std::runtime_error
{
public:
	SocketError (const std::string& what) : std::runtime_error(what) {}
};