#pragma once
#include <memory>

using std::shared_ptr;

class Response;
class Request;

class RequestHandler {
public:
    virtual ~RequestHandler() = default;
    virtual shared_ptr<Response> handle(const Request& request) = 0;
};