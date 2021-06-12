#pragma once

#include <string>
#include <unordered_map>

#include "CaseInsensitiveString.h"

// Technially all methods are defined in the HTTP Method Registry(http://www.iana.org/assignments/http-methods/http-methods.xhtml)
enum class HttpMethod
{
	Get,
	Head,
	Post,
	Put,
	Delete,
	Trace,
	Options,
	Connect,
	Patch,
};

enum class HttpVersion
{
	Http10,
	Http11,
};

class HttpRequest
{
public:
	HttpMethod method;
	std::string requestTarget;
	HttpVersion version;

	std::unordered_map<CaseInsensitiveString, std::string> headers;

	std::string body;
};