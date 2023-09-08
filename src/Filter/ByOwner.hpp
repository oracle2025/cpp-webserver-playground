#pragma once
#include "Data/Record.hpp"
#include "Data/Todo.hpp"

#include <memory>
#include <vector>

namespace Filter {

using std::map;
using std::shared_ptr;
using std::vector;

class ByOwner : public Record {
public:
    ByOwner(const Http::Request& request);
    ByOwner();
    ByOwner(const string& userId);
    string key() const override;
    vector<KeyStringType> fields() const override;
    map<KeyStringType, string> values() const override;
    HtmlInputType inputType(const KeyStringType& field) const override;
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
    Todo m_todo;
    string m_userId;
};

} // namespace Filter
