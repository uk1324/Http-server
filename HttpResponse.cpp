#include "HttpResponse.h"

HttpResponse::HttpResponse()
	: m_statusCode(HttpStatus::Ok)
	, m_statusText("OK")
{}

std::string HttpResponse::toString()
{
	std::stringstream output;

	output << "HTTP/1.1 " << static_cast<int>(m_statusCode) << " " << statusCodeToString(m_statusCode) << "\r\n";
	for (auto& [name, value] : headers)
		output << name << ": " << value << "\r\n";
	output << "\r\n";
			
	return output.str();
}

void HttpResponse::setStatus(HttpStatus statusCode)
{
	m_statusCode = statusCode;
	m_statusText = statusCodeToString(statusCode);
}

HttpStatus HttpResponse::statusCode() const
{
	return m_statusCode;
}

const std::string& HttpResponse::statusText() const
{
	return m_statusText;
}