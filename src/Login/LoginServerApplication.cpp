#include "LoginServerApplication.hpp"

#include "Http/RequestDispatcher.hpp"
#include "Impl/PocoWebServer.hpp"
#include "LoginController.hpp"
#include "Signup/SignupController.hpp"
#include "User/PasswordChangeController.hpp"
#include "doctest.h"

using std::make_shared;

int LoginServerApplication::main(const vector<string>& args)
{
    auto secretHandlers = RequestHandlerList{
        make_shared<CrudController<SimpleWebServer, Todo>>("/todo"),
        make_shared<PasswordChangeController<SimpleWebServer>>("/password")};
#ifdef ENABLE_USER_LIST
    secretHandlers.push_back(
        make_shared<CrudController<SimpleWebServer, User>>("/user"));
#endif
    shared_ptr<RequestHandler> publicHandler = nullptr;
#ifdef ENABLE_SIGNUP
    publicHandler
        = make_shared<Signup::SignupController<SimpleWebServer>>("/signup");
#endif
    LoginController<PocoWebServer> server(
        make_shared<RequestDispatcher>(secretHandlers),
        publicHandler,
        std::make_shared<Presentation>());
    server.start();
    waitForTerminationRequest();
    server.stop();
    return EXIT_OK;
}
