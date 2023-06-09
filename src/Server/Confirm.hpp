#pragma once
#include "Data/Record.hpp"
#include "Http/Response.hpp"

using std::shared_ptr;

class Confirm {
public:
    Confirm(const string& prefix, const Record& todo, const string& description);
    shared_ptr<Http::Response> operator()();

private:
    const string prefix;
    const string description;
    const Record& todo;
};
