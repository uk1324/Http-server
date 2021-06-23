#include "HttpRequest.h"

HttpRequest::HttpRequest()
	: method(HttpMethod::Get)
	, version(HttpVersion::Http11)
{}
