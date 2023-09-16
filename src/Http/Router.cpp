#include "Router.hpp"

#include "Http/NotFoundHandler.hpp"
#include "Http/Request.hpp"

namespace Http {

handler_type& Router::findHandlerOrReturnDefault(
    const RouteId& path, handler_type& defaultHandler)
{
    auto it = this->find(path);
    if (it == this->end()) {
        return defaultHandler;
    }
    return it->second;
}
Router& Router::get(const std::string& path, handler_type handler)
{
    (*this)[path] = handler;
    return *this;
}
shared_ptr<Response> Router::handle(const Request& request)
{
    return findHandlerOrReturnDefault({request.method(), request.path()}, NotFoundHandler)(request);
}
Router& Router::post(const string& path, handler_type handler)
{
    (*this)[{Http::Method::POST, path}] = handler;
    return *this;
}
} // namespace Http