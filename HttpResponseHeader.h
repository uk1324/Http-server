#pragma once

#include <unordered_map>
#include <string>
#include <iostream>

class HttpResponseHeader
{
public:
	int m_statusCode = 200;
	std::string m_statusText = "OK";
	std::unordered_map<std::string, std::string> m_headers;

public:
	std::string toString();
};