
#include "Event.hpp"

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