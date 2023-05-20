#include "CrudServerApplication.hpp"

#include "CrudComponent.hpp"
#include "Impl/PocoWebServer.hpp"

int CrudServerApplication::main(const vector<string>& args)
{
    //CrudComponent<PocoWebServer, Todo> server;
    CrudComponent<PocoWebServer, RecordImpl<TodoDefinition>> server;
    server.start();
    waitForTerminationRequest();
    server.stop();
    return EXIT_OK;
}
