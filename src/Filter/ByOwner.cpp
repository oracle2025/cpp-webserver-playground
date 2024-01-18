#include "ByOwner.hpp"

#include "Http/Session.hpp"

#include <algorithm>

namespace Filter {
string ByOwner::key() const
{
    return m_todo.key();
}
std::vector<KeyStringType> ByOwner::fields() const
{
    return m_todo.fields();
}
std::map<KeyStringType, string> ByOwner::values() const
{
    return m_todo.values();
}
HtmlInputType ByOwner::inputType(const KeyStringType& field) const
{
    return m_todo.inputType(field);
}
string ByOwner::presentableName() const
{
    return m_todo.presentableName();
}
vector<KeyStringType> ByOwner::presentableFieldsImpl() const
{
    return Todo::presentableFields();
}
void ByOwner::setImpl(const KeyStringType& field, const string& value)
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
bool ByOwner::update()
{
    return m_todo.update();
}
bool ByOwner::erase()
{
    return m_todo.erase();
}
string ByOwner::getImpl(const KeyStringType& field) const
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
string ByOwner::descriptionImpl() const
{
    return m_todo.description();
}
ByOwner::ByOwner(const Http::Request& request)
        : m_todo(request)
{
    using Http::Session;

    m_userId = Session(request).userId();
}
} // namespace Filter