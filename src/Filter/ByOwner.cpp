#include "ByOwner.hpp"

#include "Http/Session.hpp"

#include <algorithm>

namespace Filter {
ByOwner::ByOwner()
{
}
ByOwner::ByOwner(const string& userId)
{
}
string ByOwner::key() const
{
    return m_todo.key();
}
std::vector<string> ByOwner::fields() const
{
    return m_todo.fields();
}
std::map<string, string> ByOwner::values() const
{
    return m_todo.values();
}
HtmlInputType ByOwner::inputType(const string& field) const
{
    return m_todo.inputType(field);
}
string ByOwner::presentableName()
{
    return Todo::presentableName();
}
vector<string> ByOwner::presentableFields()
{
    return Todo::presentableFields();
}
void ByOwner::set(const string& field, const string& value)
{
    m_todo.set(field, value);
}
bool ByOwner::pop(const string& query)
{
    if(m_todo.pop(query)) {
        if(m_todo.values()["user_id"] != m_userId){
            m_todo.reset();
            return false;
        }
        return true;
    }
    return false;
}
void ByOwner::insert()
{
    m_todo.insert();
}
void ByOwner::update()
{
    m_todo.update();
}
void ByOwner::erase()
{
    m_todo.erase();
}
string ByOwner::get(const string& field) const
{
    return m_todo.get(field);
}
vector<shared_ptr<Record>> ByOwner::listAsPointers()
{
    auto result = m_todo.listAsPointers();
    // using remove_if method to move all the odd elements
    // to the end and get the new logical end
    auto userId = m_userId;
    auto new_logical_end = std::remove_if(
        result.begin(), result.end(), [userId](shared_ptr<Record> a) {
            return a->values()["user_id"] != userId;
        });
    result.erase(new_logical_end, result.end());
    return result;
}
string ByOwner::description() const
{
    return m_todo.description();
}
ByOwner::ByOwner(const Http::Request& request)
{
    using Http::Session;

    m_userId = Session(request).userId();
}
} // namespace Filter