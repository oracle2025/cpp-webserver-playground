#pragma once

#include "Record.hpp"

namespace Http {
class Request;
}

namespace Data {
using std::map;
using std::shared_ptr;
using std::tuple;
using std::vector;

class ScaffoldRecord : public Record {
public:
    using FieldsType = vector<tuple<KeyStringType, HtmlInputType>>;
    ScaffoldRecord() = delete;
    ScaffoldRecord(const Http::Request&);
    ScaffoldRecord(string name, FieldsType fields);
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

    void initFromCsv(std::istream& iss);
    bool find_and_pop(const KeyStringType& field, const string& value);

private:
    string id;
    string m_name;
    map<KeyStringType, string> m_values;
    FieldsType m_fields;
    static map<string, shared_ptr<ScaffoldRecord>> m_cache;
};
} // namespace Data