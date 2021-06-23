#pragma once

#include <string>
#include <unordered_map>
#include <stdexcept>

class HttpRequestTarget
{
public:
	HttpRequestTarget() = default;
	explicit HttpRequestTarget(const std::string& requestTargetString);
	const std::string& path() const;
	const std::string& query() const;

private:
	void splitRequestTarget(const std::string& requestTargetString);

private:
	std::string m_path;
	std::string m_query;
};