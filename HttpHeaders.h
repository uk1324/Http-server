#pragma once

// functions like contentLength
#include <unordered_map>

#include "CaseInsensitiveString.h"

class HttpHeaders
{
public:
	HttpHeaders() = default;

	std::string& operator[] (const CaseInsensitiveString& headerName);
	std::string& operator[] (CaseInsensitiveString&& headerName);
	const std::string& tryGetHeader(const CaseInsensitiveString headerName) const;
	void setHeaderInt(const CaseInsensitiveString& headerName, int value);
	int tryGetHeaderInt(const CaseInsensitiveString& headerName) const;
	bool containsHeader(const CaseInsensitiveString& headerName) const;

private:
	using HeaderContainer = std::unordered_map<CaseInsensitiveString, std::string>;

public:
	using iterator = HeaderContainer::iterator;
	using const_iterator = HeaderContainer::const_iterator;
	iterator begin();
	iterator end();
	const_iterator begin() const;
	const_iterator end() const;

private:
	HeaderContainer m_headers;
};

