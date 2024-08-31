#pragma once

#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wdisabled-optimization"
#include "date/date.h"
#pragma GCC diagnostic pop

#include <iosfwd>
namespace Poco::Data {
class Date;
} // namespace Poco::Data

namespace DateTime {

class Date {
public:
    Date() = delete;
    Date(const Poco::Data::Date& data);
    Date(long year, long month, long day);

    static Date currentDate();

    std::string formatAsDate() const;
    std::string formatAsDayMonth() const;
    std::string formatAsWeekday() const;

    long year() const;
    long month() const;
    long day() const;

private:
    long m_year;
    long m_month;
    long m_day;
};

} // namespace DateTime
