#include "timeDifference.hpp"

#include <sstream>

namespace String {
void parseTime(const std::string& timeStr, struct tm& timeStruct)
{
    memset(&timeStruct, 0, sizeof(struct tm)); // Clear the structure
    strptime(timeStr.c_str(), "%H:%M", &timeStruct);
    // Set the date part to a fixed value
    timeStruct.tm_year = 100; // Year 2000
    timeStruct.tm_mon = 0; // January
    timeStruct.tm_mday = 1; // 1st day of the month
}

std::string printDifference(int totalMinutes)
{
    int hours = totalMinutes / 60;
    int minutes = totalMinutes % 60;
    std::ostringstream str;
    str << hours << " h " << minutes << " min";
    return str.str();
}
std::string timeDifference(const std::string& time1, const std::string& time2)
{
    struct tm tm1 = {}, tm2 = {};
    parseTime(time1, tm1);
    parseTime(time2, tm2);

    // Convert tm structures to time_t
    time_t t1 = mktime(&tm1);
    time_t t2 = mktime(&tm2);

    // Calculate the difference in seconds
    double difference = difftime(t2, t1);

    // Convert difference to minutes
    int diffMinutes = static_cast<int>(difference / 60);

    // Print the difference
    auto result = printDifference(diffMinutes);
    return result;
}

} // namespace String