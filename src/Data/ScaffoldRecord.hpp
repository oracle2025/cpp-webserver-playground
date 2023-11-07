#pragma once

#include "Record.hpp"

namespace Data {
using std::map;
using std::shared_ptr;
using std::tuple;
using std::vector;

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
    [[nodiscard]] string get(const KeyStringType& field) const;
    vector<shared_ptr<Record>> listAsPointers();
    [[nodiscard]] std::map<KeyStringType, string> values() const override;
    [[nodiscard]] HtmlInputType inputType(
        const KeyStringType& field) const override;
    [[nodiscard]] string description() const;

private:
    string id;
    string m_name;
    map<KeyStringType, string> m_values;
    vector<tuple<KeyStringType, HtmlInputType>> m_fields;
    static map<string, shared_ptr<ScaffoldRecord>> m_cache;
};
}  // namespace Data