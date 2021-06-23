#include "HttpHeaders.h"

std::string& HttpHeaders::operator[](const CaseInsensitiveString& headerName)
{
    return m_headers[headerName];
}

std::string& HttpHeaders::operator[](CaseInsensitiveString&& headerName)
{
    return m_headers[headerName];
}

const std::string& HttpHeaders::tryGetHeader(const CaseInsensitiveString headerName) const
{
    try
    {
        return  m_headers.at(headerName);
    }
    catch (std::exception& error)
    {
        throw error;
    }
}

void HttpHeaders::setHeaderInt(const CaseInsensitiveString& headerName, int value)
{
    m_headers[headerName] = std::to_string(value);
}

int HttpHeaders::tryGetHeaderInt(const CaseInsensitiveString& headerName) const
{
    try
    {
        return std::stoi(tryGetHeader(headerName));
    }
    catch (const std::exception& error)
    {
        throw error;
    }
}

bool HttpHeaders::containsHeader(const CaseInsensitiveString& headerName) const
{
    return m_headers.count(headerName) > 0;
}

HttpHeaders::iterator HttpHeaders::begin()
{
    return m_headers.begin();
}

HttpHeaders::iterator HttpHeaders::end()
{
    return m_headers.end();
}

HttpHeaders::const_iterator HttpHeaders::begin() const
{
    return m_headers.begin();
}

HttpHeaders::const_iterator HttpHeaders::end() const
{
    return m_headers.end();
}
