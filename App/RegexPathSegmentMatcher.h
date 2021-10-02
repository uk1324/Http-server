#pragma once

#include "PathSegmentMatcher.h"

class RegexPathSegmentMatcher : PathSegmentMatcher
{
	bool match(std::string_view segment) final;
};

