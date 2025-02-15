#include "RouteId.hpp"

namespace Http {
RouteId::RouteId(const Http::Method& method, string path)
    : m_method(method)
    , m_path(std::move(path))
{
}
RouteId::RouteId(string path)
    : m_path(std::move(path))
{
}

bool RouteId::operator<(const RouteId& rhs) const
{
    if (m_method != rhs.m_method) {
        return m_method < rhs.m_method;
    }
    return m_path < rhs.m_path;
}

const string& RouteId::path() const
{
    return m_path;
}
} // namespace Http