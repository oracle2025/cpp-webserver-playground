#pragma once
#include "FieldTypes.hpp"

#include <map>
#include <string>
#include <vector>

using std::string;

class Record {
public:
    virtual ~Record() = default;
    virtual string id() const = 0;
    virtual std::vector<string> fields() const = 0;
    virtual std::map<string, string> values() const = 0;
    virtual HtmlInputType inputType(const string& field) const = 0;
};
