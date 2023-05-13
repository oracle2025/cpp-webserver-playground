#include "Router.hpp"
namespace Http {

handler_type& Router::findHandlerOrReturnDefault(
    const std::string& path, handler_type& defaultHandler)
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
} // namespace Http