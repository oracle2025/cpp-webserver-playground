#pragma once

#include "Data/Record.hpp"

#include <memory>
#include <string>
#include <vector>

namespace Html {
using std::shared_ptr;
using std::string;
using std::vector;

class List {
public:
    explicit List(vector<shared_ptr<Record>> records, vector<string> columns);
    string operator()();
    List& prefix(const string& prefix);
    List& withHeader();

private:
    vector<std::shared_ptr<Record>> records;
    vector<string> columns;
    string m_prefix;
    bool m_withHeader = false;
};

} // namespace Html
