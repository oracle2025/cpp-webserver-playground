#include "TestServer.hpp"

#include <Poco/URI.h>

shared_ptr<Response> TestServer::handle(const Request& request)
{
    Poco::URI url(request.path());
    auto path = url.getPath();
    auto query = url.getQuery();
    if (router.find(path) != router.end()) {
        auto handler = router[path];
        return handler(request);
    }
    return m_defaultHandler(request);
}
