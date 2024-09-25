#pragma once

#include <iosfwd>

namespace DateTime {

class Month {
public:
    explicit Month(int month);
    std::string asString() const;

private:
    Month() = delete;
    Month(const Month&) = delete;
    Month& operator=(const Month&) = delete;
    Month(Month&&) = delete;
    Month& operator=(Month&&) = delete;

    int m_month;
};

} // namespace DateTime
