#pragma once

#include <iosfwd>
#include <Poco/Data/Date.h>

namespace String {

tm convertDateToTm(const std::string& date);

std::string currentDateTime();

std::string localDate();

std::string localTime();

std::string currentDate();

std::string localDateTime();

std::string convertDate(const std::string& date, const std::string& format);

std::string convertDateToDayMonth(const std::string& date);
std::string convertDateToWeekday(const std::string& date);
std::string convertDateToDayMonth(const Poco::Data::Date& date);
std::string convertDateToWeekday(const Poco::Data::Date& date);

} // namespace String
