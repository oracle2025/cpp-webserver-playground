#pragma once
#include "CrudController.hpp"

class UserAdminController : public CrudController {
public:
    UserAdminController(const string& prefix, make_record_func makeRecordFunc);
protected:
    std::shared_ptr<Response> newRecord(const Request& request) override;
    std::shared_ptr<Response> createRecord(const Request& request) override;
};
