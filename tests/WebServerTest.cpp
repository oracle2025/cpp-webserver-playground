#include "Server/TestServer.hpp"
#include "doctest.h"
TEST_CASE("Web Server")
{
    TestServer w;
    w.router().get("/", [](const Request& request) { return Http::content("Hello World"); });
}