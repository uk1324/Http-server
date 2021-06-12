#include "HttpResponseHeader.h"

std::string HttpResponseHeader::toString()
{
	std::string response;
	response.reserve(100);
	response = "HTTP/1.1 " + std::to_string(m_statusCode) + ' ' + m_statusText + "\r\n";
	for (const auto& [key, value] : m_headers) {
		response += key + ": " + value + "\r\n";
	}
	response += "\r\n";
	return response;
}