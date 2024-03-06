#pragma once

#include "Data/Record.hpp"

namespace Http {
class Request;
}

namespace Data {
using std::map;
using std::shared_ptr;
using std::tuple;
using std::vector;

class ScaffoldRecord : public RecordExtended {
public:
    using FieldsType = vector<tuple<KeyStringType, HtmlInputType>>;
    ScaffoldRecord() = delete;
    ScaffoldRecord(const Http::Request&);
    ScaffoldRecord(string name, FieldsType fields);
    [[nodiscard]] string key() const override;
    [[nodiscard]] std::vector<KeyStringType> fields() const override;
    [[nodiscard]] string presentableName() const override;
    vector<KeyStringType> presentableFieldsImpl() const override;
    void setImpl(const KeyStringType& field, const string& value) override;
    bool pop(const string& query) override;
    void insert() override;
    bool update() override;
    bool erase() override;
    [[nodiscard]] string getImpl(const KeyStringType& field) const override;
    vector<shared_ptr<Record>> listAsPointers() override;
    [[nodiscard]] std::map<KeyStringType, string> values() const override;
    [[nodiscard]] HtmlInputType inputType(
        const KeyStringType& field) const override;
    [[nodiscard]] string descriptionImpl() const override;

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