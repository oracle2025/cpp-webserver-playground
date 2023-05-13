#pragma once
#include <memory>

using std::shared_ptr;
namespace Http {
struct Response;
struct Request;

class RequestHandler {
public:
    virtual ~RequestHandler() = default;
    virtual shared_ptr<Response> handle(const Request& request) = 0;
};
} // namespace Http