#include "Data/Date.hpp"
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
