
#include "SimpleWebServer.hpp"
void SimpleWebServer::finish_init()
{
}
shared_ptr<Response> SimpleWebServer::handle(const Request& request)
{
    return m_router.findHandlerOrReturnDefault({request.method(), request.path()}, m_defaultHandler)(request);
}
void SimpleWebServer::defaultHandler(handler_type handler)
{
    m_defaultHandler = std::move(handler);
}
void SimpleWebServer::setPresentation(
    shared_ptr<Html::Presentation> presentation)
{
}
Router& SimpleWebServer::router()
{
    return m_router;
}
