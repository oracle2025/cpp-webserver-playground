#include "RequestDispatcher.hpp"

#include "Http/NotFoundHandler.hpp"

namespace Http {

RequestDispatcher::RequestDispatcher(RequestHandlerList handlers)
    : m_handlers(move(handlers))
{
}
shared_ptr<Response> RequestDispatcher::handle(const Request& request)
{
    for (auto& handler : m_handlers) {
        auto response = handler->handle(request);
        if (response) {
            return response;
        }
    }
    return Http::NotFoundHandler(request);
}

} // namespace Http