#pragma once

#include <iosfwd>

namespace String {

std::string currentDateTime();

std::string localDate();

std::string currentDate();

std::string localDateTime();

std::string convertDate(const std::string& date, const std::string& format);

std::string convertDateToDayMonth(const std::string& date);
std::string convertDateToWeekday(const std::string& date);

} // namespace String
