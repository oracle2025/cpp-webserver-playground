#include "NotFoundHandler.hpp"

#include "Router.hpp"
#include "Response.hpp"
namespace Http {
handler_type NotFoundHandler = [](const Request&) {
    return content("404 not found")
        ->code(Response::NOT_FOUND)
        .shared_from_this();
};
}