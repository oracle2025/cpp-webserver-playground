#include "Data/Date.hpp"
#include "String/currentDateTime.hpp"
#include "String/timeDifference.hpp"
#include "doctest.h"

TEST_CASE("Calculate time difference")
{
    std::string time1, time2;

    // Example input
    time1 = "11:00";
    time2 = "12:30";


    // Print the difference
    auto result = String::timeDifference(time1, time2);

    CHECK_EQ(result, "1 h 30 min");
}

TEST_CASE("Calculate Hours Minutes")
{


}
static std::tm convertDateToTm(const std::string& date)
{
    using namespace date;
    using namespace std::chrono;
    std::istringstream in(date);
    sys_days tm;
    in >> parse("%Y-%m-%d", tm);

    if (in.fail() || in.bad() || in.eof()) {
        throw std::runtime_error("Invalid date");
    }

    // convert sys_days to tm
    time_t time = system_clock::to_time_t(tm);
    std::tm t = *std::localtime(&time);
    return t;
}
TEST_CASE("Convert Date To Weekday")
{
    //CHECK_EQ(String::convertDateToWeekday("2024-07-21"), "Montag");
    CHECK_EQ(convertDateToTm("2024-07-21").tm_year, 2024);
    CHECK_EQ(convertDateToTm("2024-07-01").tm_year, 2024);
    CHECK_EQ(convertDateToTm("2024-07-02").tm_year, 2024);
    CHECK_EQ(convertDateToTm("2024-07-03").tm_year, 2024);
    CHECK_EQ(convertDateToTm("2024-07-04").tm_year, 2024);
    CHECK_EQ(convertDateToTm("2024-07-05").tm_year, 2024);
}