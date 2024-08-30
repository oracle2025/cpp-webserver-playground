#pragma once
#include <iosfwd>
namespace Poco::Data {
class Time;
} // namespace Poco::Data
namespace DateTime {

class Time {
public:
    Time(const Poco::Data::Time &data);
    Time(long h, long m);
    long toMinutes() const;
    Time difference(const Time& other) const;
    static Time parseTime(const std::string& timeStr);
    std::string formatAsTotalHours() const;
    std::string formatAsTime() const;
    void add(const Time& other);

private:
    long hours;
    long minutes;
};

} // namespace Time
