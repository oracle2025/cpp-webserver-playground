#include "LoginServerApplication.hpp"

#include "Impl/PocoWebServer.hpp"
#include "LoginController.hpp"
#include "doctest.h"

int LoginServerApplication::main(const vector<string>& args)
{
    LoginController<PocoWebServer> server(
        std::make_shared<CrudController<SimpleWebServer, Todo>>("/todo"),
        std::make_shared<Presentation>());
    server.start();
    waitForTerminationRequest();
    server.stop();
    return EXIT_OK;
}
