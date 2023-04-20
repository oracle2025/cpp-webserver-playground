#pragma once

#include "Record.hpp"

#include <memory>
#include <string>
#include <vector>

namespace Html {
using std::shared_ptr;
using std::string;
using std::vector;

class List {
public:
    explicit List(vector<shared_ptr<Record>> records);
    string operator()();

private:
    vector<std::shared_ptr<Record>> records;
};

} // namespace Html
