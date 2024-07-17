
#include "currentDateTime.hpp"

#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wdisabled-optimization"
#include "date/date.h"
#pragma GCC diagnostic pop

#include "Trace/trace.hpp"
#include "doctest.h"

#include <ctime>
#include <iomanip>
#include <map>

namespace String {

std::string localDateTime()
{
    auto now = std::chrono::system_clock::now();
    std::time_t now_time_t = std::chrono::system_clock::to_time_t(now);
    std::tm local_time = *std::localtime(&now_time_t);
    std::ostringstream str;
    str << std::put_time(&local_time, "%Y-%m-%d %H:%M:%S");
    return str.str();
}
// Current Date time in the format 2024-06-13 08:45:00
std::string currentDateTime()
{
    using namespace date;
    using namespace std::chrono;
    auto now = floor<seconds>(system_clock::now());
    std::ostringstream str;
    str << format("%F %T", now);
    return str.str();
}

TEST_CASE("compare Local and current Time")
{
        //CHECK_EQ(localDateTime(), currentDateTime());
}

// Current Date time in the format 2024-06-13
std::string currentDate()
{
    using namespace date;
    using namespace std::chrono;
    auto now = floor<seconds>(system_clock::now());
    std::ostringstream str;
    str << format("%F", now);
    return str.str();
}
std::string localDate()
{
    auto now = std::chrono::system_clock::now();
    std::time_t now_time_t = std::chrono::system_clock::to_time_t(now);
    std::tm local_time = *std::localtime(&now_time_t);
    std::ostringstream str;
    str << std::put_time(&local_time, "%Y-%m-%d");
    return str.str();
}
std::string convertDate(const std::string& date, const std::string& format_str)
{
    using namespace date;
    using namespace std::chrono;
    std::istringstream in(date);
    sys_days tm;
    in >> parse("%Y-%m-%d", tm);

    if (in.fail() || in.bad() || in.eof()) {
        TRACE_THROW("Invalid date");
    }

    std::ostringstream str;
    str << format(format_str, tm);
    return str.str();
}
std::tm convertDateToTm(const std::string& date)
{
    using namespace date;
    using namespace std::chrono;
    std::istringstream in(date);
    sys_days tm;
    in >> parse("%Y-%m-%d", tm);

    if (in.fail() || in.bad() || in.eof()) {
        TRACE_THROW("Invalid date");
    }

    // convert sys_days to tm
    time_t time = system_clock::to_time_t(tm);
    std::tm t = *std::localtime(&time);
    return t;
}
std::string convertDateToDayMonth(const std::string& date)
{
    static const std::map<int, std::string> month_names
        = {{1, "Januar"},
           {2, "Februar"},
           {3, "März"},
           {4, "April"},
           {5, "Mai"},
           {6, "Juni"},
           {7, "Juli"},
           {8, "August"},
           {9, "September"},
           {10, "Oktober"},
           {11, "November"},
           {12, "Dezember"}};
    std::ostringstream str;
    std::tm t = convertDateToTm(date);
    str << t.tm_mday << ". " << month_names.at(t.tm_mon + 1);
    return str.str();
}
std::string convertDateToWeekday(const std::string& date)
{
    static const std::map<int, std::string> week_days
        = {{1, "Montag"},
           {2, "Dienstag"},
           {3, "Mittwoch"},
           {4, "Donnerstag"},
           {5, "Freitag"},
           {6, "Samstag"},
           {7, "Sonntag"}};
    std::ostringstream str;
    std::tm t = convertDateToTm(date);
    str << week_days.at(t.tm_wday);
    return str.str();
}

} // namespace String