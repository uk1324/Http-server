#pragma once

#include <string>

class PathMatcher
{
public:
	explicit PathMatcher(const std::string& path);

	// Use string_view for spliting strings
	// Maybe make a function
	bool match(std::string path);
private:
	std::string m_string;
	//class Segment
	//{
	//public:
	//	bool match();
	//	bool isAny();
	//};
};

