#include "LoginServerApplication.hpp"

#include "Header.hpp"
#include "Impl/PocoWebServer.hpp"
#include "LoginServer.hpp"
#include "doctest.h"

int LoginServerApplication::main(const vector<string>& args)
{
    LoginServer<PocoWebServer> server(
        std::make_shared<CrudServer<RecursiveWebServer>>(),
        std::make_shared<Presentation>(Header("<html>"), Footer("</html>")));
    server.start();
    waitForTerminationRequest();
    server.stop();
    return EXIT_OK;
}
