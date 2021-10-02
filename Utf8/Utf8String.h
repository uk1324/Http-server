#pragma once

#include <cstdint>
#include <vector>

class Utf8String
{
public:
	Utf8String();
	Utf8String(const char* string);
	~Utf8String();
	size_t length();
	uint32_t& operator[] (size_t index);

private:
	uint8_t* m_data;
	size_t m_size;
	size_t m_capacity;

	size_t m_length;
	uint8_t* m_pointer;

	//mutable std::vector<size_t> cachedCodePointsIndices;
	//mutable bool isCacheValid;
};

