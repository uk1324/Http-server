#pragma once

#include "../Http/HttpServer.h"
#include "PathMatcher.h"
#include "RequestHandler.h"

#include <memory>

class HttpApp : public HttpServer
{
public:
	void get(const std::string& path, std::shared_ptr<RequestHandler> requestHandler);

private:
	void handleRequest(TcpSocket& socket, const HttpRequest& request) override;
	void handleInvalidRequest(TcpSocket& socket, HttpRequestParser::Error error) override;

private:
	using Endpoint = std::pair<PathMatcher, std::shared_ptr<RequestHandler>>;

	std::vector<Endpoint> m_getEndpoints;
};

