#include "TestServer.hpp"

#include "doctest.h"
#include "overloaded.hpp"

#include <Poco/URI.h>

string TestServer::getPage(
    const string& path,
    const map<string, string>& cookies,
    const map<string, string>& params)
{
    if (router.find(path) != router.end()) {
        auto& handler = router[path];
        Request parameters(path, cookies, params, "");
        return handler(parameters)->content();
    }
    return "";
}
shared_ptr<Response> TestServer::getResponse(
    const string& uri,
    const map<string, string>& cookies,
    const map<string, string>& params)
{
    Poco::URI url(uri);
    auto path = url.getPath();
    auto query = url.getQuery();
    if (router.find(path) != router.end()) {
        auto& handler = router[path];
        Request parameters(path, cookies, params, query);
        return handler(parameters);
    }
    return m_defaultHandler(Request(path, cookies, params, query));
}
shared_ptr<Response> TestServer::postTo(
    const string& path, const map<string, string>& params)
{
    if (router.find(path) != router.end()) {
        auto& handler = router[path];
        Request request(path, {}, params, "");
        return handler(request);
    }
    return m_defaultHandler(Request(path, {}, params, ""));
}

TEST_CASE("Web Server")
{
    TestServer w;
    w.get("/", [](const Request& request) { return content("Hello World"); });
}