#include "CalendarController.hpp"

#include "Http/Request.hpp"
#include "Http/Response.hpp"
#include "Http/Router.hpp"

#include <sstream>

using Http::content;
using Http::Request;
using Http::Response;
using Http::Router;

namespace Calendar {

void CalendarController::initialize(const std::string& prefix, Router& router)
{
    router.get(prefix + "/", [](const Request& request) {
        std::ostringstream calendar;
        calendar << R"(<table class="table table-bordered">)" << "\n";
        calendar << R"(<tr>)" << "\n";
        calendar << R"(<th style="width:50px;">Mo</th>)" << "\n";
        calendar << R"(<th style="width:50px;">Di</th>)" << "\n";
        calendar << R"(<th style="width:50px;">Mi</th>)" << "\n";
        calendar << R"(<th style="width:50px;">Do</th>)" << "\n";
        calendar << R"(<th style="width:50px;">Fr</th>)" << "\n";
        calendar << R"(<th style="width:50px;">Sa</th>)" << "\n";
        calendar << R"(<th style="width:50px;">So</th>)" << "\n";
        calendar << R"(</tr>)" << "\n";
        const int days_in_month = 31;
        for (int i = 0; i < 5; i++) {
            calendar << R"(<tr>)" << "\n";
            for (int j = 0; j < 7; j++) {
                const auto day = 1 + j + (i * 7);
                if (day >= days_in_month) {
                    calendar << R"(<td></td>)" << "\n";
                } else if (j >= 5) {
                    calendar << R"(<td class="active">)" << day
                             << R"(</td>)" << "\n";
                } else {
                    calendar << R"(<td>)" << day << R"(</td>)" << "\n";
                }
            }
            calendar << R"(</tr>)" << "\n";
        }
        calendar << R"(</table>)" << "\n";
        return content(calendar.str());
    });
}

} // namespace Calendar