#pragma once
#include "Data/Event.hpp"
#include "Email/SendEmail.hpp"
#include "Events/EventList.hpp"
#include "Events/WeeklyAgenda.hpp"
#include "Http/Request.hpp"
#include "Http/Response.hpp"
#include "NullHandler.hpp"
#include "Server/WebServer.hpp"

#include <ginger.h>
namespace Events {

using Http::content;

template<typename T>
class CalendarController : public T {
public:
    using Response = Http::Response;
    using Request = Http::Request;
    CalendarController(const string& prefix)
    {
        static_assert(
            std::is_base_of<WebServer, T>::value,
            "T not derived from WebServer");
        T::router().get(prefix + "/", [](const Request& request) {
            std::ostringstream str;
            str << "<table border='1px solid black'>";
            str << "<tr>";
            str << "<th>Mo</th>";
            str << "<th>Tu</th>";
            str << "<th>We</th>";
            str << "<th>Th</th>";
            str << "<th>Fr</th>";
            str << "<th>Sa</th>";
            str << "<th>Su</th>";
            str << "</tr>";
            str << "<tr>";
            for (int i = 1; i <= 31; i++) {
                str << "<td>" << i << "</td>";
                if (i % 7 == 0) {
                    str << "</tr><tr>";
                }
            }
            str << "</tr>";
            str << "</table>";
            const string html_template = R"(<h1>${title}</h1>)";
            std::map<std::string, ginger::object> template_values
                = {{"title", "Hello Template"}};
            ginger::parse(
                html_template, template_values, ginger::from_ios(str));

            return content(str.str())
                ->appendNavBarAction({"Start", "/"})
                .title("Calendar")
                .shared_from_this();
        });
        T::router().get(
            prefix + "/send_weekly_agenda", [](const Request& request) {
                EventList events{Data::Event(request).listAsPointers()};
                const auto agenda = Events::WeeklyAgenda(events);
                const auto rendered = agenda.render();
                Email::SendEmail("", "", "").sendHTML("", "", "", rendered);
                return content("send_weekly_agenda")
                    ->appendNavBarAction({"Start", "/"})
                    .title("Calendar")
                    .shared_from_this();
            });
        T::router().get(prefix + "/weekly_agenda", [](const Request& request) {
            EventList events{Data::Event(request).listAsPointers()};
            const auto agenda = Events::WeeklyAgenda(events);
            const auto rendered = agenda.render();
            return content(rendered)
                ->appendNavBarAction({"Start", "/"})
                .title("Calendar")
                .shared_from_this();
        });
        T::defaultHandler(Http::NullHandler);
        T::finish_init();
    }
};

} // namespace Events
