#pragma once

#include <iosfwd>

namespace String {

std::string currentDateTime();

std::string currentDate();

std::string localDateTime();

std::string convertDate(const std::string& date, const std::string& format);

} // namespace String
