#pragma once

#include "Record.hpp"
#include "Request.hpp"
namespace Data {
using std::vector;
using std::shared_ptr;

class ScaffoldRecord : public Record {
};

class Event : public ScaffoldRecord {
public:
    Event() = default;
    explicit Event(const Http::Request& request);

    [[nodiscard]] string key() const override;
    [[nodiscard]] std::vector<KeyStringType> fields() const override;
    [[nodiscard]] std::map<KeyStringType, string> values() const override;
    [[nodiscard]] HtmlInputType inputType(
        const KeyStringType& field) const override;
    static string presentableName();
    static vector<KeyStringType> presentableFields();
    void set(const KeyStringType& field, const string& value);
    bool pop(const string& query);
    void insert();
    void update();
    void erase();
    string get(const KeyStringType& field) const;
    vector<shared_ptr<Record>> listAsPointers();
    string description() const;

private:
    string id;
    string subject;
    string startDate;
    string endDate;
    string startTime;
    string endTime;
};

}