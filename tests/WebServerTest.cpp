#include "TestServer.hpp"
#include "doctest.h"
TEST_CASE("Web Server")
{
    TestServer w;
    w.get("/", [](const Request& request) { return content("Hello World"); });
}