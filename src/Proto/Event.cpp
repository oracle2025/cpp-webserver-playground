
#include "Proto/Event.hpp"

#include <doctest.h>

#include <sstream>

namespace Data {

Event::Event(const Http::Request&)
    : ScaffoldRecord(
          "event",
          {{"subject", HtmlInputType::TEXT},
           {"startDate", HtmlInputType::DATE},
           {"endDate", HtmlInputType::DATE},
           {"startTime", HtmlInputType::TIME},
           {"endTime", HtmlInputType::TIME}})
{
}
} // namespace Data

TEST_CASE("Event")
{
    Data::Event event_items{Http::Request{}};
    std::istringstream csv(R"(subject,startDate,endDate,startTime,endTime
First of January,2021-01-01,2021-01-01,00:00,23:59
First of February,2021-02-01,2021-02-01,00:00,23:59
First of March,2021-03-01,2021-03-01,00:00,23:59
First of April,2021-04-01,2021-04-01,00:00,23:59
)");
    event_items.initFromCsv(csv);
    CHECK(event_items.listAsPointers().size() == 4);
}