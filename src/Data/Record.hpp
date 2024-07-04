#pragma once
#include "FieldTypes.hpp"

#include <map>

#ifndef __clang__
#   pragma GCC diagnostic push
#   pragma GCC diagnostic ignored "-Wmaybe-uninitialized"
#endif
#include <regex>
#ifndef __clang__
#   pragma GCC diagnostic pop
#endif
#include <string>
#include <vector>

using std::string;
struct KeyStringType : public string {
    KeyStringType(const string& s)
        : string(s)
    {
        // throw if invalid char
        // allowed: a-z, A-Z, 0-9, _
        if (s.find_first_not_of("abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRST"
                                "UVWXYZ0123456789_")
            != string::npos) {
            throw std::invalid_argument(
                "Invalid character in KeyStringType: " + s);
        }
    }
    KeyStringType(const char* s)
        : string(s)
    {
        std::regex r("[a-zA-Z0-9_]+");
        if (!std::regex_match(s, r)) {
            throw std::invalid_argument(
                "Invalid character in KeyStringType: " + string(s));
        }
    }
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