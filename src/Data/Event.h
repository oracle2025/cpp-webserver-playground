#pragma once

#include "Record.hpp"
#include "Request.hpp"
namespace Data {
using std::shared_ptr;
using std::tuple;
using std::vector;
using std::map;

class ScaffoldRecord : public Record {
public:
    ScaffoldRecord() = delete;
    ScaffoldRecord(
        string name, vector<tuple<KeyStringType, HtmlInputType>> fields);
    [[nodiscard]] string key() const override;
    [[nodiscard]] std::vector<KeyStringType> fields() const override;
    string presentableName();
    vector<KeyStringType> presentableFields();
    void set(const KeyStringType& field, const string& value);
    bool pop(const string& query);
    void insert();
    void update();
    void erase();
    string get(const KeyStringType& field) const;
    vector<shared_ptr<Record>> listAsPointers();
    [[nodiscard]] std::map<KeyStringType, string> values() const override;
    [[nodiscard]] HtmlInputType inputType(
        const KeyStringType& field) const override;
    string description() const;

private:
    string id;
    string m_name;
    map<KeyStringType, string> m_values;
    vector<tuple<KeyStringType, HtmlInputType>> m_fields;
    static map<string, shared_ptr<ScaffoldRecord>> m_cache;
};
struct Event : public ScaffoldRecord {
    Event() = delete;
    Event(const Http::Request& request)
        : ScaffoldRecord(
            "event",
            {{"subject", HtmlInputType::TEXT},
             {"startDate", HtmlInputType::DATE},
             {"endDate", HtmlInputType::DATE},
             {"startTime", HtmlInputType::TIME},
             {"endTime", HtmlInputType::TIME}})
    {
    }
};

} // namespace Data