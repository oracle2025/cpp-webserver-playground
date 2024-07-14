#pragma once
#include <iosfwd>
namespace Time {

class Time {
public:
    Time(long h, long m);
    long toMinutes() const;
    Time difference(const Time& other) const;
    static Time parseTime(const std::string& timeStr);
     std::string toString() const;
    void add(const Time& other);

private:
    long hours;
    long minutes;
};

} // namespace Time
