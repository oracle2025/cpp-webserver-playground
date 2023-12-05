#pragma once

#include <memory>
#include <vector>
namespace Data {
struct Event;
}
class Record;
namespace Events {

class EventList : public std::vector<std::shared_ptr<Data::Event>> {
public:
    EventList(const std::vector<std::shared_ptr<Record>> &records);
};

} // namespace Events
