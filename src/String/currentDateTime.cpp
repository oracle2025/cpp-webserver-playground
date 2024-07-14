
#include "currentDateTime.hpp"

#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wdisabled-optimization"
#include "date/date.h"
#pragma GCC diagnostic pop

#include "Trace/trace.hpp"

#include <ctime>
#include <iomanip>

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

} // namespace String