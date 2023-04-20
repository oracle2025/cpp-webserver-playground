#include "TestServer.hpp"
#include "overloaded.hpp"
#include "doctest.h"

string TestServer::getPage(
    const string& path,
    const map<string, string>& cookies,
    const map<string, string>& params)
{
    if (router.find(path) != router.end()) {
        auto &handler = router[path];
        auto result = visit(overloaded{
                                [](response_handler_type &handler) { return handler()->content(); },
                                [&cookies, &params](request_response_handler_type &handler) {
                                    Request parameters(cookies, params, "");
                                    return handler(parameters)->content();
                                }},
                            handler);
        return result;
    }
    return "";
}
shared_ptr<Response> TestServer::getResponse(
    const string& path,
    const map<string, string>& cookies,
    const map<string, string>& params)
{
    if (router.find(path) != router.end()) {
        auto &handler = router[path];
        auto result = visit(overloaded{
                                [](response_handler_type &handler) { return handler(); },
                                [&cookies, &params](request_response_handler_type &handler) {
                                    Request parameters(cookies, params, "");
                                    return handler(parameters);
                                }},
                            handler);
        return result;
    }
    return content("");
}
shared_ptr<Response> TestServer::postTo(
    const string& path, const map<string, string>& params)
{
    if (router.find(path) != router.end()) {
        return visit(overloaded{
                         [](response_handler_type &handler) { return handler(); },
                         [params](request_response_handler_type &handler) {
                             Request request({}, params, "");
                             return handler(request);
                         }},
                     router[path]);
    }
    return content("");
}


TEST_CASE("Web Server") {
    TestServer w;
    w.get("/", [] { return content("Hello World"); });
}