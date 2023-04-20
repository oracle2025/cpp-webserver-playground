#include "CrudServerApplication.hpp"

#include "CrudServer.hpp"
#include "PocoWebServer.hpp"

int CrudServerApplication::main(const vector<string>& args)
{
    CrudServer<PocoWebServer> server;
    server.start();
    waitForTerminationRequest();
    server.stop();
    return EXIT_OK;
}
