#pragma once

#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wdisabled-optimization"
#include "date/date.h"
#pragma GCC diagnostic pop

#include <string>

namespace Data {

class Date {
public:
    Date() = delete;
    explicit Date(const std::string&);
    std::string render() const;
    // less than operator
    bool operator<(const Date& rhs) const;
    /*bool operator==(const Date& rhs) const;
    bool operator!=(const Date& rhs) const;
    bool operator>(const Date& rhs) const;
    bool operator<=(const Date& rhs) const;
    bool operator>=(const Date& rhs) const;*/
private:
    [[nodiscard]] date::sys_days get() const;

    date::sys_days m_date;
};

} // namespace Data
