#include "Date.hpp"

#include "doctest.h"
#include "Trace/trace.hpp"

#include <chrono>
#include <iomanip>
#include <sstream>

using std::string;

string to_date_and_back_again(const string& input)
{
    using namespace date;
    using namespace std::chrono;
    std::istringstream in(input);
    sys_days tm;
    in >> parse("%d.%m.%Y", tm);

    if (in.fail() || in.bad() || in.eof()) {
        TRACE_THROW("Invalid date");
    }

    std::ostringstream str;
    str << format("%d.%m.%Y", tm);
    return str.str();
}

TEST_CASE("Parsing Dates")
{
    CHECK("09.01.2014" == to_date_and_back_again("09.01.2014"));
    CHECK_THROWS(to_date_and_back_again("33.01.2014"));
    CHECK_THROWS(to_date_and_back_again("09.13.2014"));
}

namespace Data {
Date::Date(const std::string& initializer)
{
    using namespace date;
    using namespace std::chrono;
    std::istringstream in(initializer);
    sys_days tm;
    in >> parse("%d.%m.%Y", tm);

    if (in.fail() || in.bad() || in.eof()) {
        TRACE_THROW("Invalid date");
    }

    m_date = tm;
}
std::string Date::render() const
{
    std::ostringstream str;
    str << date::format("%d.%m.%Y", m_date);
    return str.str();
}
date::sys_days Date::get() const
{
    return m_date;
}
bool Date::operator<(const Date& rhs) const
{
    return m_date < rhs.m_date;
}
} // namespace Data