#include "LoginServerApplication.hpp"

#include "Http/RequestDispatcher.hpp"
#include "Impl/PocoWebServer.hpp"
#include "LoginController.hpp"
#include "User/PasswordChangeComponent.hpp"
#include "doctest.h"

using std::make_shared;

int LoginServerApplication::main(const vector<string>& args)
{

    LoginController<PocoWebServer> server(
        make_shared<RequestDispatcher>(RequestHandlerList{
            make_shared<CrudController<SimpleWebServer, Todo>>("/todo"),
            make_shared<PasswordChangeComponent<SimpleWebServer>>(
                "/password")}),
        std::make_shared<Presentation>());
    server.start();
    waitForTerminationRequest();
    server.stop();
    return EXIT_OK;
}
