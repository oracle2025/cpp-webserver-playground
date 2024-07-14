#include "Time.hpp"

#include <sstream>

namespace Time {
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
std::string Time::toString() const
{
    std::ostringstream str;
    str << hours << " h " << minutes << " min";
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