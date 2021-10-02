#include <string_view>

class PathSegmentMatcher
{
public:
	virtual bool match(std::string_view segment) = 0;
	virtual ~PathSegmentMatcher() = 0;
};