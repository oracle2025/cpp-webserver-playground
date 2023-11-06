#pragma once
#include "Http/Request.hpp"
#include "Http/Response.hpp"
#include "NullHandler.hpp"
#include "Server/WebServer.hpp"
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
            return content(str.str())
                ->appendNavBarAction({"Start", "/"})
                .title("Calendar")
                .shared_from_this();
        });
        T::defaultHandler(Http::NullHandler);
        T::finish_init();
    }
};

} // namespace Events
