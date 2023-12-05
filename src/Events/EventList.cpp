#include "EventList.hpp"
#include "Data/Event.hpp"

namespace Events {
EventList::EventList(const std::vector<std::shared_ptr<Record>>& records)
{
    clear();
    for (const auto& record : records) {
        push_back(std::dynamic_pointer_cast<Data::Event>(record));
    }
}
} // namespace Events