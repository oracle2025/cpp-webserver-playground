#pragma once
#include "Data/Record.hpp"
#include "Data/Todo.hpp"

#include <memory>
#include <vector>

namespace Filter {

using std::map;
using std::shared_ptr;
using std::vector;

class ByOwner : public RecordExtended {
public:
    ByOwner(const Http::Request& request);
    string key() const override;
    vector<KeyStringType> fields() const override;
    map<KeyStringType, string> values() const override;
    HtmlInputType inputType(const KeyStringType& field) const override;
    string presentableName() const override;
    vector<KeyStringType> presentableFieldsImpl() const override;
    void setImpl(const KeyStringType& field, const string& value) override;
    bool pop(const string& query) override;
    void insert() override;
    bool update() override;
    bool erase() override;
    string getImpl(const KeyStringType& field) const override;
    vector<shared_ptr<Record>> listAsPointers() override;
    string descriptionImpl() const override;

private:
    Todo m_todo;
    string m_userId;
};

} // namespace Filter
