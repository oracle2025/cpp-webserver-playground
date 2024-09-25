#include "Month.hpp"
#include <map>
#include <string>
#include "Trace/trace.hpp"

namespace DateTime {
Month::Month(int month)
: m_month(month)
{
    if (m_month < 1 || m_month > 12) {
        TRACE_THROW("Invalid month: " + std::to_string(m_month));
    }
}
std::string Month::asString() const
{
    static const std::map<int, std::string> month_names
        = {{1, "Januar"},
           {2, "Februar"},
           {3, "März"},
           {4, "April"},
           {5, "Mai"},
           {6, "Juni"},
           {7, "Juli"},
           {8, "August"},
           {9, "September"},
           {10, "Oktober"},
           {11, "November"},
           {12, "Dezember"}};
    if (month_names.find(m_month) == month_names.end()) {
        return {};
    }
    return month_names.at(m_month);
}
} // namespace DateTime