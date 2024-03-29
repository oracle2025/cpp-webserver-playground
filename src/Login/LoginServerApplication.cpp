#include "LoginServerApplication.hpp"

#include "Data/SharedTodo.hpp"
#include "Filter/ByOwner.hpp"
#include "Http/RequestDispatcher.hpp"
#include "Impl/PocoWebServer.hpp"
#include "LoginController.hpp"
#include "NullHandler.hpp"
#include "Server/CrudController.hpp"
#include "Signup/SignupController.hpp"
#include "SimpleWebServer.hpp"
#include "TodoController.hpp"
#include "User/PasswordChangeController.hpp"
#include "doctest.h"

#include <inja.hpp>

using Http::RequestDispatcher;
using Http::RequestHandlerList;
using std::make_shared;

int LoginServerApplication::main(const vector<string>& args)
{
    auto handler = std::make_shared<SimpleWebServer>();
    PasswordChangeController password_change_controller(
        "/password", handler->router());
    CrudController sharedCrud(
        "/shared",
        [](const Request& request) {
            return std::make_shared<SharedTodo>(request);
        },
        handler->router());
    TodoController todoCrud(
        "/todo",
        [](const Request& request) {
            return std::make_shared<Filter::ByOwner>(request);
        },
        handler->router());
    handler->router().get("/", [](const Request& request) {
        std::ostringstream out;
        try {
            const auto tpl = TEMPLATE_DIR "/home.html";
            inja::Environment env;
            const inja::Template temp = env.parse_template(tpl);
            out << env.render(temp, {});
        } catch (...) {
            TRACE_RETHROW("Could not render template");
        }
        return content(out.str())
            ->appendNavBarAction({"Start", "/"})
            .shared_from_this();
    });

    handler->defaultHandler(Http::NullHandler);
    handler->finish_init();

    shared_ptr<SimpleWebServer> adminHandler = nullptr;
#ifdef ENABLE_USER_LIST
    adminHandler = make_shared<SimpleWebServer>();
    CrudController userCrud(
        "/user",
        [](const Request& request) {
            return std::make_shared<Data::User>(request);
        },
        adminHandler->router());
    adminHandler->defaultHandler(Http::NullHandler);
    adminHandler->finish_init();
#endif
    shared_ptr<SimpleWebServer> publicHandler = nullptr;
#ifdef ENABLE_SIGNUP
    publicHandler = std::make_shared<SimpleWebServer>();
    Signup::SignupController signup_controller(
        "/signup", publicHandler->router());
    publicHandler->defaultHandler(Http::NullHandler);
    publicHandler->finish_init();
#endif
    PocoWebServer server2;
    auto presentation = std::make_shared<Presentation>();
    LoginController server(
        handler, adminHandler, publicHandler, presentation, server2.router());
    server2.defaultHandler(server.getDefaultHandler());
    server2.setPresentation(presentation);
    server2.finish_init();

    server2.start();
    waitForTerminationRequest();
    server2.stop();
    return EXIT_OK;
}
