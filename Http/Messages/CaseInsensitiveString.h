#pragma once

#include <string>
#include <ostream>

struct CaseInsensitiveCharTraits : public std::char_traits<char>
{
	static bool lt(char one, char two);
    static bool eq(char one, char two);
    static int compare(const char* one, const char* two, size_t length);
};

using CaseInsensitiveString = std::basic_string<char, CaseInsensitiveCharTraits>;

std::ostream& operator<< (std::ostream& os, const CaseInsensitiveString& str);

namespace std
{
    template <> struct hash<CaseInsensitiveString>
    {
        size_t operator()(const CaseInsensitiveString& string) const
        {
            std::hash<char> hasher;

            size_t hash = 0;
            for (char chr : string)
                hash = hasher(static_cast<char>(std::tolower(chr)));

            return hash;
        }
    };
}