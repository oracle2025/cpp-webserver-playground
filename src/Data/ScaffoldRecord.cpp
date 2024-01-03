#include "ScaffoldRecord.hpp"

#include "String/capitalize.hpp"
#include "String/createRandomUUID.hpp"
#include "String/split.hpp"
#include "doctest.h"

#include <sstream>

namespace Data {
ScaffoldRecord::ScaffoldRecord(const Http::Request&)
{
    throw std::runtime_error("Not implemented");
}
ScaffoldRecord::ScaffoldRecord(
    string name, FieldsType fields)
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
    return String::capitalize(m_name);
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
void ScaffoldRecord::initFromCsv(std::istream& iss)
{
    // first line is the header

    string header;
        std::getline(iss, header);
    auto fields = String::split(header, ",");

    for (std::string line; std::getline(iss, line); )
    {
        auto values_as_strings = String::split(line, ",");
        for (size_t i = 0; i < fields.size(); ++i) {
            set(fields[i], values_as_strings[i] );
        }
        insert();
    }
}
bool ScaffoldRecord::find_and_pop(
    const KeyStringType& field, const string& value)
{
    for (auto& [_, record] : m_cache) {
        if (record->get(field) == value) {
            m_values = record->m_values;
            return true;
        }
    }
    return false;
}

TEST_CASE("Init Scaffold Record with CSV")
{
    struct ShoppingItem : public ScaffoldRecord {
        ShoppingItem()
            : ScaffoldRecord(
                  "shopping_item",
                  {{"description", HtmlInputType::TEXT},
                   {"price", HtmlInputType::TEXT}})
        {
        }
    };
    ShoppingItem item;
    auto csv = std::istringstream{R"(description,price
salad,1.99
eggs,2.99
peppers,3.99
)"};
    item.initFromCsv(csv);
    CHECK(item.find_and_pop("description", "salad"));
    CHECK(item.get("description") == "salad");
    CHECK(item.get("price") == "1.99");
    CHECK(item.find_and_pop("description", "eggs"));
    CHECK(item.get("description") == "eggs");
    CHECK(item.get("price") == "2.99");
    CHECK(item.find_and_pop("description", "peppers"));
    CHECK(item.get("description") == "peppers");
    CHECK(item.get("price") == "3.99");
}

} // namespace Data