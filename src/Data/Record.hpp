#pragma once
#include "FieldTypes.hpp"

#include <map>


#include <string>
#include <vector>
#include <memory>

using std::string;
struct KeyStringType : public string {
    KeyStringType(const string& s);
    KeyStringType(const char* s);
};
class Record {
public:
    virtual ~Record() = default;
    [[nodiscard]] virtual string key() const = 0;
    [[nodiscard]] virtual std::vector<KeyStringType> fields() const = 0;
    [[nodiscard]] virtual std::map<KeyStringType, string> values() const = 0;
    [[nodiscard]] virtual HtmlInputType inputType(
        const KeyStringType& field) const
        = 0;
    [[nodiscard]] virtual string presentableName() const = 0;
};
struct RecordExtended : public Record {
    virtual void insert() = 0;
    virtual bool update() = 0;
    virtual bool pop(const string& _id) = 0;
    virtual void setImpl(const KeyStringType& key, const string& value) = 0;
    virtual bool erase() = 0;
    [[nodiscard]] virtual string descriptionImpl() const = 0;
    [[nodiscard]] virtual std::vector<KeyStringType> presentableFieldsImpl() const = 0;
    virtual std::vector<std::shared_ptr<Record>> listAsPointers() = 0;
    virtual string getImpl(const KeyStringType& key) const = 0;
};