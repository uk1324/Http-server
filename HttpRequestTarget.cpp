#include "HttpRequestTarget.h"

#include <iostream>

HttpRequestTarget::HttpRequestTarget(const std::string& requestTargetString)
{
	splitRequestTarget(requestTargetString);
}

const std::string& HttpRequestTarget::path() const
{
	return m_path;
}

const std::string& HttpRequestTarget::query() const
{
	return m_query;
}

void HttpRequestTarget::splitRequestTarget(const std::string& requestTargetString)
{
	size_t i = 0;

	while ((i < requestTargetString.length()) && (requestTargetString[i] != '?'))
	{
		m_path += requestTargetString[i];
		i++;
	}
	// Skip question mark
	i++;

	while (i < requestTargetString.length())
	{
		m_query += requestTargetString[i];
		i++;
	}	
}