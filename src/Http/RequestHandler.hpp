#pragma once
#include <memory>

namespace Http {
using std::shared_ptr;
class Response;
class Request;

class RequestHandler {
public:
    virtual ~RequestHandler() = default;
    virtual shared_ptr<Response> handle(const Request& request) = 0;
};
} // namespace Http