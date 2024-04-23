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

void makeCruds(std::shared_ptr<SimpleWebServer> handler)
{
    auto sharedCrud = std::make_shared<CrudController>(
                          "/shared",
                          [](const Request& request) {
                              return std::make_shared<SharedTodo>(request);
                          })
                          ->initialize(handler->router())
                          .shared_from_this();
    auto todoCrud = std::make_shared<TodoController>(
                        "/todo",
                        [](const Request& request) {
                            return std::make_shared<Filter::ByOwner>(request);
                        })
                        ->initialize(handler->router())
                        .shared_from_this();
}

int LoginServerApplication::main(const vector<string>& args)
{
    auto handler = std::make_shared<SimpleWebServer>();
    PasswordChangeController::initialize("/password", handler->router());
    makeCruds(handler);
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

    handler->defaultHandler(Http::NotFoundHandler);
    handler->finish_init();

    shared_ptr<SimpleWebServer> adminHandler = nullptr;
#ifdef ENABLE_USER_LIST
    adminHandler = make_shared<SimpleWebServer>();
    auto userCrud = std::make_shared<CrudController>(
                        "/user",
                        [](const Request& request) {
                            return std::make_shared<Data::User>(request);
                        })
                        ->initialize(adminHandler->router())
                        .shared_from_this();
    adminHandler->defaultHandler(Http::NullHandler);
    adminHandler->finish_init();
#endif
    shared_ptr<SimpleWebServer> publicHandler = nullptr;
#ifdef ENABLE_SIGNUP
    publicHandler = std::make_shared<SimpleWebServer>();
    Signup::SignupController::initialize("/signup", publicHandler->router());
    publicHandler->defaultHandler(Http::NullHandler);
    publicHandler->finish_init();
#endif
    // Goal: Extract the generation of LoginController to a separate Method,
    // So I can test it without needing to init PocoWebServer
    PocoWebServer server2;
    auto presentation = std::make_shared<Presentation>();
    auto server = std::make_shared<LoginController>(
                      handler, adminHandler, publicHandler, presentation)
                      ->initialize(server2.router())
                      .shared_from_this();
    server2.defaultHandler(server->getDefaultHandler());
    server2.setPresentation(presentation);
    server2.finish_init();

    server2.start();
    waitForTerminationRequest();
    server2.stop();
    return EXIT_OK;
}
