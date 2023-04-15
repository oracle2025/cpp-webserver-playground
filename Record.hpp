#pragma once
#include <string>
#include <vector>
#include <map>

using std::string;

class Record {
public:
    virtual ~Record() = default;
    virtual std::vector<string> fields() const = 0;
    virtual std::map<string, string> values() const = 0;
};


