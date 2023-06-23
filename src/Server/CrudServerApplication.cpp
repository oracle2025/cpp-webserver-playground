#include "CrudServerApplication.hpp"

#include "CrudController.hpp"
#include "Impl/PocoWebServer.hpp"

int CrudServerApplication::main(const vector<string>& args)
{
    CrudController<PocoWebServer, Todo> server("/todo");
    server.start();
    waitForTerminationRequest();
    server.stop();
    return EXIT_OK;
}
