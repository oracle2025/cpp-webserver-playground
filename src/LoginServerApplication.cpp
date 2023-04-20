#include "LoginServerApplication.hpp"
#include "PocoWebServer.hpp"
#include "LoginServer.hpp"
int LoginServerApplication::main(const vector<string>& args)
{
    LoginServer<PocoWebServer> server;
    server.start();
    waitForTerminationRequest();
    server.stop();
    return EXIT_OK;
}
