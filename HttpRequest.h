#pragma once

#include <string>
#include <unordered_map>

#include "CaseInsensitiveString.h"
#include "HttpRequestTarget.h"
#include "HttpHeaders.h"

// Technially all methods are defined in the HTTP Method Registry (http://www.iana.org/assignments/http-methods/http-methods.xhtml)
enum class HttpMethod
{
	Get,
	Post,
	Put,
	Delete,
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
	HttpRequest();

	HttpMethod method;
	HttpRequestTarget requestTarget;
	HttpVersion version;

	HttpHeaders headers;

	std::string body;
};