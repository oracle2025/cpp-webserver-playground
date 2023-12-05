#include "WeeklyAgenda.hpp"

#include "Data/Date.hpp"
#include "Data/Event.hpp"
#include <set>

namespace Events {
WeeklyAgenda::WeeklyAgenda(EventList events)
    : m_events(std::move(events))
{
}
string WeeklyAgenda::render() const
{
    /*
    *
Montag, 27. November
    Geburtstag Moritz
    Taschengeld Mattis
Dienstag, 28. November
    Come to Office for Sprint
Donnerstag, 30. November
    11:00 Werkstatttermin More Cargo Bike
Freitag, 31. November
    BSR Müllabfuhr
    Geburtstag Simone
    MonatsCheckliste
        Kontaktlinsen wechseln
        Gartengeld in Vogelhäuschen
     */
    /*
     * List days
     * sort chronologically
     * print day
     * and corresponding events
     */
    std::set<Data::Date> dates;
    for (auto event : m_events) {
        Data::Date startDate(event->get("startDate"));
        dates.insert(startDate);
    }
    return R"(<ul>
<li>Montag, 27. November
<ul>
    <li>Geburtstag Moritz</li>
    <li>Taschengeld Mattis</li>
</ul></li>
<li>Dienstag, 28. November
<ul>
    Come to Office for Sprint</li>
</ul></li>
<li>Donnerstag, 30. November
<ul>
    <li>11:00 Werkstatttermin More Cargo Bike</li>
</ul></li>
<li>Freitag, 31. November
<ul>
    <li>BSR Müllabfuhr</li>
    <li>Geburtstag Simone</li>
    <li>MonatsCheckliste
    <ul>
        Kontaktlinsen wechseln</li>
        Gartengeld in Vogelhäuschen</li>
    </ul></li>
</ul></li>
</ul>)";
}
} // namespace Events