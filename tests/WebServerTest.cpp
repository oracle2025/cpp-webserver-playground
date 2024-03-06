#include "Impl/SimpleWebServer.hpp"
#include "doctest.h"
TEST_CASE("Web Server")
{
    SimpleWebServer w;
    w.router().get("/", [](const Request& request) { return Http::content("Hello World"); });
}