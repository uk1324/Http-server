#pragma once

#include <stdexcept>

// Could just put this in socket base
class SocketError : public std::runtime_error
{
public:
	SocketError (const std::string& what) : std::runtime_error(what) {}
};