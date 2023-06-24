#pragma once

#include "Http/RequestHandler.hpp"

#include <memory>
#include <vector>

namespace Http {

using std::shared_ptr;
using std::vector;

using RequestHandlerList = vector<shared_ptr<RequestHandler>>;

class RequestDispatcher : public RequestHandler {
public:
    RequestDispatcher(RequestHandlerList handlers);
    shared_ptr<Response> handle(const Request& request) override;

private:
    RequestHandlerList m_handlers;
};

} // namespace Http
