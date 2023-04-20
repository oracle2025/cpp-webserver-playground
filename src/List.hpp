#pragma once

#include "Record.hpp"
#include <string>
#include <vector>
#include <memory>

namespace Html {
using std::string;
using std::vector;
using std::shared_ptr;

class List {
public:
    explicit List(vector<shared_ptr<Record>> records);
    string operator()();

private:
    vector<std::shared_ptr<Record>> records;
};

}// namespace Html
