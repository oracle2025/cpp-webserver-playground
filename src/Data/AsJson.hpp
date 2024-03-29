#pragma once

#include <nlohmann/json.hpp>

class Record;

namespace Data {

class AsJson {
public:
    static nlohmann::json fromRecord(const Record& record);


};

} // Data

