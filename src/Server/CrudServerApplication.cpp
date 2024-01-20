#include "CrudServerApplication.hpp"

#include "CrudController.hpp"
#include "Impl/PocoWebServer.hpp"

int CrudServerApplication::main(const vector<string>& args)
{
    PocoWebServer server;
    CrudController<Todo> controller(
        "/todo",
        [](const Request& request) { return std::make_shared<Todo>(request); },
        server.router());
    server.defaultHandler(Http::NullHandler);
    server.finish_init();
    server.start();
    waitForTerminationRequest();
    server.stop();
    return EXIT_OK;
}
