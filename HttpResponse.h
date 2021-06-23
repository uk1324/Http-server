#pragma once

#include <unordered_map>
#include <string>
#include <iostream>
#include <sstream>

#include "HttpStatus.h"
#include "CaseInsensitiveString.h"

class HttpResponse
{
public:
	HttpResponse();

	void setStatus(HttpStatus statusCode);
	HttpStatus statusCode() const;
	const std::string& statusText() const;

	std::string toString();

public:
	std::unordered_map<CaseInsensitiveString, std::string> headers;

private:
	HttpStatus m_statusCode = HttpStatus::Ok;
	std::string m_statusText = "OK";
};