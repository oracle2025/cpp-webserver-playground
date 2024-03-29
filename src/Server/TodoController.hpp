#pragma once
#include "CrudController.hpp"

class TodoController : public CrudController {
public:
    TodoController(
        const string& prefix,
        make_record_func makeRecordFunc,
        Http::Router& router);
protected:
    std::shared_ptr<Response> editRecord(const Request& request);

};
