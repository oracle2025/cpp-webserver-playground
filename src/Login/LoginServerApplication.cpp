#include "LoginServerApplication.hpp"

#include "Impl/PocoWebServer.hpp"
#include "LoginComponent.hpp"
#include "doctest.h"

int LoginServerApplication::main(const vector<string>& args)
{
    LoginComponent<PocoWebServer> server(
        std::make_shared<CrudComponent<SimpleWebServer, Todo>>(),
        std::make_shared<Presentation>());
    server.start();
    waitForTerminationRequest();
    server.stop();
    return EXIT_OK;
}
