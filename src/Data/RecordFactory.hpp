#pragma once

#include <memory>

using std::shared_ptr;
class Record;

class RecordFactory {
public:
    virtual ~RecordFactory() = default;
    virtual shared_ptr<Record> create() = 0;
};