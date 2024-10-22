#include "doctest.h"

TEST_CASE("Generate Weekly Overview")
{
    auto input = R"(
        Donnerstag, 31.10.2024: Wasserdienst
        Donnerstag, 7.11.2024: Wasserdienst
    )";
    auto expected_cw_44 = R"(
        Woche 44, vom 28. Oktober bis 1. November:

        * Donnerstag:
    )";
}