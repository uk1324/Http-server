#include "HttpApp.h"

void HttpApp::get(const std::string& path, std::shared_ptr<RequestHandler> requestHandler)
{
	m_getEndpoints.push_back(std::make_pair(PathMatcher(path), requestHandler));
}

void HttpApp::handleRequest(TcpSocket& socket, const HttpRequest& request)
{
	Request req(request);
	Response res(socket);
	

	switch (request.method)
	{
	case HttpMethod::Get:
		for (auto& [pathMatcher, requestHandler] : m_getEndpoints)
		{
			if (pathMatcher.match(req.requestTarget.path()))
				requestHandler->handleRequest(req, res);

			if (res.isFinished())
				break;
		}
		if (res.isFinished() == false)
		{
			res.send("Couldn't get " + req.requestTarget.path());
		}
		break;

	default:
		break;
	}
}

void HttpApp::handleInvalidRequest(TcpSocket& socket, HttpRequestParser::Error error)
{
}
