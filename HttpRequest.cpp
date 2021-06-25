#include "HttpRequest.h"

HttpRequest::HttpRequest()
	: method(HttpMethod::Get)
	, version(HttpVersion::Http11)
{}

std::string HttpRequest::toString()
{
	std::stringstream output;

	//output << 

	return output.str();
}
