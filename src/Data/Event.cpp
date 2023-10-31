
#include "Event.h"

namespace Data {

string Event::key() const
{
    return id;
}
std::vector<KeyStringType> Event::fields() const
{
    return {};
}
std::map<KeyStringType, string> Event::values() const
{
    return {
        {"startDate", startDate},
        {"endDate", endDate},
        {"startTime", startTime},
        {"endTime", endTime},
        {"subject", subject}
    };
    /*
     * string subject;
string startDate;
string endDate;
string startTime;
string endTime;
     */
}
HtmlInputType Event::inputType(const KeyStringType& field) const
{
    const map<KeyStringType, HtmlInputType> inputTypes = {
        {"subject", HtmlInputType::TEXT},
        {"startDate", HtmlInputType::DATE},
        {"endDate", HtmlInputType::DATE},
        {"startTime", HtmlInputType::TIME},
        {"endTime", HtmlInputType::TIME}};
    if (inputTypes.find(field) != inputTypes.end()) {
        return inputTypes.at(field);
    }
    return HtmlInputType::HIDDEN;
}

Event::Event(const Http::Request& request)
{
}

string Event::presentableName()
{
    return "Event";
}
vector<KeyStringType> Event::presentableFields()
{
    return {};
}
void Event::set(const KeyStringType& field, const string& value)
{
}
bool Event::pop(const string& query)
{
    return true;
}
void Event::insert()
{
}
void Event::update()
{
}
void Event::erase()
{
}
string Event::get(const KeyStringType& field) const
{
    return {};
}
vector<shared_ptr<Record>> Event::listAsPointers()
{
    return {};
}
string Event::description() const
{
    return subject;
}
} // namespace Data