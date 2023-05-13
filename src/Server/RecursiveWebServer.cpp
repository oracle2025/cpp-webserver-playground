
#include "RecursiveWebServer.hpp"
void RecursiveWebServer::finish_init()
{
}
shared_ptr<Response> RecursiveWebServer::handle(const Request& request)
{
    return m_router.findHandlerOrReturnDefault(request.path(), m_defaultHandler)(request);
}
void RecursiveWebServer::defaultHandler(handler_type handler)
{
    m_defaultHandler = std::move(handler);
}
void RecursiveWebServer::setPresentation(
    shared_ptr<Html::Presentation> presentation)
{
}
Router& RecursiveWebServer::router()
{
    return m_router;
}
