#pragma once
#include <string>
#include <vector>
#include "EventList.hpp"

namespace Data {
struct Event;
}
namespace Events {

using Data::Event;
using std::shared_ptr;
using std::string;
using std::vector;

class WeeklyAgenda {
public:
    explicit WeeklyAgenda(EventList events);
    [[nodiscard]] string render() const;

private:
    EventList m_events;
};

} // namespace Events
