#pragma once

#include <string>
#include <unordered_map>

#include "CaseInsensitiveString.h"

// Technially all methods are defined in the HTTP Method Registry(http://www.iana.org/assignments/http-methods/http-methods.xhtml)
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
	//friend HttpRequestParser;
public:
	/*HttpMethod method() const;
	void setMethod(HttpMethod method);
	HttpVersion version() const;
	void setVersion(HttpVersion version);
	const std::string& body() const;
	std::string& body();
	const std::unordered_map<CaseInsensitiveString, std::string>& header() const;*/
	//std::string getHeader(CaseInsensitiveString name);

private:
	//HttpMethod m_method;
	//HttpVersion m_version;
	//std::unordered_map<CaseInsensitiveString, std::string> m_headers;
	//std::string m_body;

public:
	HttpMethod method;
	std::string requestTarget;
	HttpVersion version;

	std::unordered_map<CaseInsensitiveString, std::string> headers;

	std::string body;
};

//HttpMethod HttpRequest::method() const
//{
//	return m_method;
//}
// 
//void HttpRequest::setMethod(HttpMethod method)
//{
//	m_method = method;
//}

//
//HttpVersion HttpRequest::version() const
//{
//	return m_version;
//}

//void HttpRequest::setVersion(HttpVersion version)
//{
//	m_version = version;
//}
//const std::string& HttpRequest::body() const
//{
//	return m_body;
//}

//std::string& HttpRequest::body()
//{
//	return m_body;
//}

//std::string HttpRequest::getHeader(CaseInsensitiveString name)
//{
//	return m_headers[name];
//}