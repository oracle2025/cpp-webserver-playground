#pragma once
#include "date/date.h"

#include <string>

namespace Data {

class Date {
public:
    Date() = delete;
    explicit Date(const std::string&);
    std::string render() const;
    // less then operator
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
