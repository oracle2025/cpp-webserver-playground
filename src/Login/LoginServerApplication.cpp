#include "LoginServerApplication.hpp"

#include "Filter/ByOwner.hpp"
#include "Http/RequestDispatcher.hpp"
#include "Impl/PocoWebServer.hpp"
#include "LoginController.hpp"
#include "Signup/SignupController.hpp"
#include "User/PasswordChangeController.hpp"
#include "doctest.h"

using Http::RequestDispatcher;
using Http::RequestHandlerList;
using std::make_shared;

int LoginServerApplication::main(const vector<string>& args)
{
    auto secretHandlers = RequestHandlerList{
        make_shared<CrudController<SimpleWebServer, Filter::ByOwner>>("/todo"),
        make_shared<PasswordChangeController<SimpleWebServer>>("/password")};
    shared_ptr<RequestHandler> adminHandler = nullptr;
#ifdef ENABLE_USER_LIST
    adminHandler
        = make_shared<CrudController<SimpleWebServer, Data::User>>("/user");
#endif
    shared_ptr<RequestHandler> publicHandler = nullptr;
#ifdef ENABLE_SIGNUP
    publicHandler
        = make_shared<Signup::SignupController<SimpleWebServer>>("/signup");
#endif
    LoginController<PocoWebServer> server(
        make_shared<RequestDispatcher>(secretHandlers),
        adminHandler,
        publicHandler,
        std::make_shared<Presentation>());
    server.start();
    waitForTerminationRequest();
    server.stop();
    return EXIT_OK;
}
