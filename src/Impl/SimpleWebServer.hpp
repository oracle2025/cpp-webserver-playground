#pragma once
#include "Http/NotFoundHandler.hpp"
#include "Http/Request.hpp"
#include "Http/RequestHandler.hpp"
#include "Http/Response.hpp"
#include "Http/Router.hpp"

#include <functional>
#include <memory>
#include <variant>

using Http::content;
using Http::handler_type;
using Http::NotFoundHandler;
using Http::Request;
using Http::RequestHandler;
using Http::Response;
using Http::Router;
using std::function;
using std::shared_ptr;
using std::variant;
namespace Html {
struct Presentation;
}
class SimpleWebServer : public RequestHandler {
public:
    void finish_init();
    shared_ptr<Response> handle(const Request& request) override;
    void defaultHandler(handler_type handler);
    void setPresentation(shared_ptr<Html::Presentation> presentation);
    Router& router();

private:
    Router m_router;
    handler_type m_defaultHandler;
};
