#include "Time.hpp"

#include <Poco/Data/Time.h>

#include <iomanip>
#include <sstream>

namespace DateTime {
Time::Time(const Poco::Data::Time& data)
    : hours(data.hour())
    , minutes(data.minute())
{
}
Time::Time(long h, long m)
    : hours(h)
    , minutes(m)
{
}
long Time::toMinutes() const
{
    return hours * 60 + minutes;
}
Time Time::difference(const Time& other) const
{
    long diff = toMinutes() - other.toMinutes();
    return {diff / 60, diff % 60};
}
Time Time::parseTime(const std::string& timeStr)
{
    std::istringstream ss(timeStr);
    long hours, minutes;
    char colon;
    ss >> hours >> colon >> minutes;
    return {hours, minutes};
}
std::string Time::formatAsTotalHours() const
{
    std::ostringstream str;
    str << hours << " h " << minutes << " min";
    return str.str();
}
std::string Time::formatAsTime() const
{
    std::ostringstream str;
    str << std::setfill('0') << std::setw(2) << hours << ":"
        << std::setfill('0') << std::setw(2) << minutes;
    return str.str();
}
void Time::add(const Time& other)
{
    hours += other.hours;
    minutes += other.minutes;
    if (minutes >= 60) {
        hours++;
        minutes -= 60;
    }
}
} // namespace Time