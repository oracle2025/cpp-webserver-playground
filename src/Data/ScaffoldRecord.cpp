#include "ScaffoldRecord.hpp"
#include "String/createRandomUUID.hpp"

namespace Data {
ScaffoldRecord::ScaffoldRecord(
    string name, vector<tuple<KeyStringType, HtmlInputType>> fields)
    : m_name(std::move(name))
    , m_fields(std::move(fields))
{
    for(auto& [key, value] : m_fields) {
        m_values[key] = "";
    }
}
map<string, shared_ptr<ScaffoldRecord>> ScaffoldRecord::m_cache;
string ScaffoldRecord::key() const
{
    return id;
}
std::vector<KeyStringType> ScaffoldRecord::fields() const
{
    std::vector<KeyStringType> result;
    for (auto& [key, value] : m_fields) {
        result.push_back(key);
    }
    return result;
}

std::map<KeyStringType, string> ScaffoldRecord::values() const
{
    return m_values;
}
HtmlInputType ScaffoldRecord::inputType(const KeyStringType& field) const
{
    for (auto& [key, value] : m_fields) {
        if (key == field) {
            return value;
        }
    }
    return HtmlInputType::HIDDEN;
}

string ScaffoldRecord::presentableName()
{
    return m_name;
}
vector<KeyStringType> ScaffoldRecord::presentableFields()
{
    vector<KeyStringType> fields;
    fields.reserve(m_fields.size());
    for (auto& [key, value] : m_fields) {
        fields.push_back(key);
    }
    return fields;
}
void ScaffoldRecord::set(const KeyStringType& field, const string& value)
{
    m_values[field] = value;
}
bool ScaffoldRecord::pop(const string& query)
{
    if (m_cache.find(query) != m_cache.end()) {
        *this = *m_cache[query];
        return true;
    }
    return false;
}
void ScaffoldRecord::insert()
{
    id = String::createRandomUUID();
    auto record = std::make_shared<ScaffoldRecord>(m_name, m_fields);
    *record = *this;
    m_cache[id] = record;
}
void ScaffoldRecord::update()
{
    if (id.empty()) {
        insert();
        return;
    }
    auto record = std::make_shared<ScaffoldRecord>(m_name, m_fields);
    *record = *this;
    m_cache[id] = record;
}
void ScaffoldRecord::erase()
{
    if (m_cache.find(id) != m_cache.end()) {
        m_cache.erase(id);
    }
}
string ScaffoldRecord::get(const KeyStringType& field) const
{
    if (m_values.find(field) != m_values.end()) {
        return m_values.at(field);
    }
    return {};
}
vector<shared_ptr<Record>> ScaffoldRecord::listAsPointers()
{
    vector<shared_ptr<Record>> result;
    for (auto& [key, value] : m_cache) {
        if (value->m_name == m_name) {
            result.push_back(value);
        }
    }
    return result;
}
string ScaffoldRecord::description() const
{
    return m_values.at(0);
}
} // namespace Data