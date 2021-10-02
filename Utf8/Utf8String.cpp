#include "Utf8String.h"

Utf8String::Utf8String()
	: m_data(nullptr)
	, m_capacity(0)
	, m_size(0)
	, m_length(0)
{
}

Utf8String::~Utf8String()
{
	::operator delete(m_data);
}
