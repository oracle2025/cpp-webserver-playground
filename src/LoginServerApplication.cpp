#include "LoginServerApplication.hpp"

#include "LoginServer.hpp"
#include "PocoWebServer.hpp"
int LoginServerApplication::main(const vector<string>& args)
{
    LoginServer<PocoWebServer> server(
        std::make_shared<CrudServer<RecursiveWebServer>>());
    server.start();
    waitForTerminationRequest();
    server.stop();
    return EXIT_OK;
}
