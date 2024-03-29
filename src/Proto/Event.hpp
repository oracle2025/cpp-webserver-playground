#pragma once

#include "../Data/Record.hpp"
#include "../Http/Request.hpp"
#include "ScaffoldRecord.hpp"
namespace Data {
using std::map;
using std::shared_ptr;
using std::tuple;
using std::vector;


struct Event : public ScaffoldRecord {
    Event() = delete;
    explicit Event(const Http::Request&);
};

} // namespace Data