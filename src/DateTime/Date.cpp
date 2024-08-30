#include "Date.hpp"

#include "Trace/trace.hpp"
#include "doctest.h"
#include "String/currentDateTime.hpp"

#include <Poco/Data/Date.h>

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

namespace DateTime {
Date::Date(const Poco::Data::Date& data)
    : m_year(data.year())
    , m_month(data.month())
    , m_day(data.day())
{
}

std::string Date::formatAsDate() const
{
    std::ostringstream str;
    str << std::setfill('0') << std::setw(2) << m_year << "-"
        << std::setfill('0') << std::setw(2) << m_month << "-" << m_day;
    return str.str();
}
std::string Date::formatAsDayMonth() const
{
    return String::convertDateToDayMonth(formatAsDate());
}
std::string Date::formatAsWeekday() const
{
    return {};
}
} // namespace DateTime