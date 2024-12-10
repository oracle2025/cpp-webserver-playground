#include "Time.hpp"

#include <Poco/Data/Time.h>

#include <iomanip>
#include <sstream>

namespace DateTime {

void parseTime(const std::string& timeStr, struct tm& timeStruct)
{
    memset(&timeStruct, 0, sizeof(struct tm)); // Clear the structure
    strptime(timeStr.c_str(), "%H:%M", &timeStruct);
    // Set the date part to a fixed value
    timeStruct.tm_year = 100; // Year 2000
    timeStruct.tm_mon = 0; // January
    timeStruct.tm_mday = 1; // 1st day of the month
}
int timeDifference(const std::string& time1, const std::string& time2)
{
    struct tm tm1 = {}, tm2 = {};
    parseTime(time1, tm1);
    parseTime(time2, tm2);

    // Convert tm structures to time_t
    time_t t1 = mktime(&tm1);
    time_t t2 = mktime(&tm2);

    // Calculate the difference in seconds
    double difference = difftime(t2, t1);

    // Convert difference to minutes
    int diffMinutes = static_cast<int>(difference / 60);

    return diffMinutes;
}
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
bool Time::operator<=(const Time& other) const
{
    return difference(other).toMinutes() <= 0;
}
bool Time::operator>=(const Time& other) const
{
    return difference(other).toMinutes() >= 0;
}
bool Time::operator<(const Time& other) const
{
    return difference(other).toMinutes() < 0;
}
bool Time::operator>(const Time& other) const
{
    return difference(other).toMinutes() > 0;
}
std::ostream& operator<<(std::ostream& os, const Time& tm)
{
    os << tm.formatAsTime();
    return os;
}
} // namespace DateTime