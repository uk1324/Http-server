#pragma once

#include <string>
#include <unordered_map>
#include <stdexcept>

// Could take in a string and the path, query and segements would be string_views
class HttpRequestTarget
{
public:
	class ParsingError : public std::exception {};

	HttpRequestTarget() = default;
	explicit HttpRequestTarget(const std::string& requestTargetString);
	const std::string& path() const;
	const std::string& query() const;
	const std::vector<std::string>& segments() const;

private:
	void splitRequestTarget(const std::string& requestTargetString);
	void splitPath(const std::string& path);

private:
	std::string m_path;
	std::string m_query;
	std::vector<std::string> m_segments;
};