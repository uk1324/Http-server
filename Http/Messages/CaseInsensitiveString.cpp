#include "CaseInsensitiveString.h"

bool CaseInsensitiveCharTraits::lt(char one, char two)
{
    return std::tolower(one) < std::tolower(two);
}

bool CaseInsensitiveCharTraits::eq(char one, char two)
{
    return std::tolower(one) == std::tolower(two);
}

int CaseInsensitiveCharTraits::compare(const char* one, const char* two, size_t length)
{
    for (size_t i = 0; i < length; ++i) {
        if (lt(one[i], two[i]))
            return -1;

        if (lt(two[i], one[i]))
            return 1;
    }
    return 0;
}

std::ostream& operator<< (std::ostream& os, const CaseInsensitiveString& str)
{
    for (auto ch : str)
        os << static_cast<char>(ch);
    return os;
}