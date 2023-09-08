#pragma once
#include "FieldTypes.hpp"

#include <map>
#include <string>
#include <vector>
#include <regex>

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
    virtual string key() const = 0;
    virtual std::vector<KeyStringType> fields() const = 0;
    virtual std::map<KeyStringType, string> values() const = 0;
    virtual HtmlInputType inputType(const KeyStringType& field) const = 0;
};
