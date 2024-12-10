#include "DateTime/Date.hpp"
#include "DateTime/Time.hpp"
#include "String/currentDateTime.hpp"
#include "doctest.h"

TEST_CASE("Calculate time difference")
{
    std::string time1, time2;

    // Example input
    time1 = "11:00";
    time2 = "12:30";

    using DateTime::Time;
    // Print the difference
    auto result = Time::parseTime(time2)
                      .difference(Time::parseTime(time1))
                      .formatAsTotalHours();
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
    CHECK_EQ(String::convertDateToWeekday("2024-07-15"), "Montag");
    CHECK_EQ(String::convertDateToWeekday("2024-07-16"), "Dienstag");
    CHECK_EQ(String::convertDateToWeekday("2024-07-17"), "Mittwoch");
    CHECK_EQ(String::convertDateToWeekday("2024-07-18"), "Donnerstag");
    CHECK_EQ(String::convertDateToWeekday("2024-07-19"), "Freitag");
    CHECK_EQ(String::convertDateToWeekday("2024-07-20"), "Samstag");
    CHECK_EQ(String::convertDateToWeekday("2024-07-21"), "Sonntag");
}
TEST_CASE("Compare Time Less than Equal")
{
    const auto A = DateTime::Time::parseTime("10:00");
    const auto B = DateTime::Time::parseTime("11:00");
    CHECK(A <= B);
    CHECK(A <= A);
    CHECK_FALSE(B <= A);
}
TEST_CASE("Compare Time Greater than Equal")
{
    const auto A = DateTime::Time::parseTime("10:00");
    const auto B = DateTime::Time::parseTime("11:00");
    CHECK(B >= A);
    CHECK(A >= A);
    CHECK_FALSE(A >= B);
}
TEST_CASE("Compare Time Less than")
{
    const auto A = DateTime::Time::parseTime("10:00");
    const auto B = DateTime::Time::parseTime("11:00");
    CHECK(A < B);
    CHECK_FALSE(A < A);
    CHECK_FALSE(B < A);
}
TEST_CASE("Compare Time Greater than")
{
    const auto A = DateTime::Time::parseTime("10:00");
    const auto B = DateTime::Time::parseTime("11:00");
    CHECK(B > A);
    CHECK_FALSE(A > A);
    CHECK_FALSE(A > B);
}