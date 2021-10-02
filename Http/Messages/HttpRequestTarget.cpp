#include "HttpRequestTarget.h"
#include "../Parsing/HttpParsingUtils.h"

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

const std::vector<std::string>& HttpRequestTarget::segments() const
{
	return m_segments;
}

void HttpRequestTarget::splitRequestTarget(const std::string& requestTargetString)
{
	size_t i = 0;

	while ((i < requestTargetString.length()) && (requestTargetString[i] != '?'))
	{
		m_path += requestTargetString[i];
		i++;
	}
	splitPath(m_path);
	// Skip question mark
	i++;

	while (i < requestTargetString.length())
	{
		if (HttpParsingUtils::isPchar(requestTargetString[i]) == false)
			throw ParsingError();
		m_query += requestTargetString[i];
		i++;
	}	
}

void HttpRequestTarget::splitPath(const std::string& path)
{
	if (path.length() > 0 && path[0] != '/')
		throw ParsingError();

	std::string segment;

	for (size_t i = 1; i < path.length(); i++)
	{
		char chr = path[i];

		if (chr == '/')
		{
			if (segment.length() == 0)
				throw ParsingError();

			auto decoded = HttpParsingUtils::decodePercentEncodedString(segment);

			if (decoded.has_value())
			{
				m_segments.push_back(decoded.value());
			}
			else
			{
				throw ParsingError();
			}

			segment.clear();
		}
		else
		{
			if (HttpParsingUtils::isPchar(chr) == false)
				throw ParsingError();
			segment += chr;
		}
	}

	if (segment.length() != 0)
	{
		auto decoded = HttpParsingUtils::decodePercentEncodedString(segment);

		if (decoded.has_value())
		{
			m_segments.push_back(decoded.value());
		}
		else
		{
			throw ParsingError();
		}
	}
}
