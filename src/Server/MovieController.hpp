#pragma once
#include "CrudController.hpp"

class MovieController : public CrudController {
public:
    MovieController(const string& prefix, make_record_func makeRecordFunc);

protected:
    std::shared_ptr<Response> listRecords(const Request& request) override;
};
