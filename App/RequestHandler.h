#pragma once

#include "Request.h"
#include "Response.h"

class RequestHandler
{
public:
	virtual void handleRequest(Request& req, Response& res) = 0;
	virtual ~RequestHandler() = default;
};

