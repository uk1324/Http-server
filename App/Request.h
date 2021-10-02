#pragma once

#include "../Http/Messages/HttpRequest.h"

class Request : public HttpRequest
{
public:
	explicit Request(const HttpRequest& request);
};