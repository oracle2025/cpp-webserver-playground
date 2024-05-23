#include "LoginServerApplication.hpp"

#include "Data/Movie.hpp"
#include "Data/SharedTodo.hpp"
#include "Document/DocumentController.hpp"
#include "Filter/ByOwner.hpp"
#include "Http/RequestHandlerList.hpp"
#include "Impl/PocoWebServer.hpp"
#include "Login/ProfileController.hpp"
#include "LoginController.hpp"
#include "MovieController.hpp"
#include "NullHandler.hpp"
#include "Server/CrudController.hpp"
#include "Session.hpp"
#include "Signup/SignupController.hpp"
#include "SimpleWebServer.hpp"
#include "TodoController.hpp"
#include "User/PasswordChangeController.hpp"
#include "doctest.h"

#include <inja.hpp>

using Http::RequestHandlerList;
using std::make_shared;

void makeCruds(Http::Router& router)
{
    auto sharedCrud = std::make_shared<CrudController>(
                          "/shared",
                          [](const Request& request) {
                              return std::make_shared<SharedTodo>(request);
                          })
                          ->initialize(router)
                          .shared_from_this();
    auto todoCrud = std::make_shared<TodoController>(
                        "/todo",
                        [](const Request& request) {
                            return std::make_shared<Filter::ByOwner>(request);
                        })
                        ->initialize(router)
                        .shared_from_this();
    auto movieCrud = std::make_shared<MovieController>(
                         "/movie",
                         [](const Request& request) {
                             return std::make_shared<Data::Movie>(request);
                         })
                         ->initialize(router)
                         .shared_from_this();
}
void makeHome(Http::Router& router)
{
    using Data::User;
    using Http::redirect;
    router.get("/", [](const Request& request) {
        auto user = std::make_shared<User>();
        if (!user->pop(Http::Session(request).userId())) {
            throw std::runtime_error("Logged in User not found");
        }
        if (user->values()["start_page"] != "/") {
            return redirect(user->values()["start_page"]);
        } else {
            std::ostringstream out;
            try {
                const auto tpl = TEMPLATE_DIR "/home.html";
                inja::Environment env;
                const inja::Template temp = env.parse_template(tpl);
                out << env.render(temp, {});
            } catch (...) {
                TRACE_RETHROW("Could not render template");
            }
            return content(out.str());
        }
    });
}
std::shared_ptr<SimpleWebServer> makeAdminHandler()
{
    auto adminHandler = make_shared<SimpleWebServer>();
    auto userCrud = std::make_shared<CrudController>(
                        "/user",
                        [](const Request& request) {
                            return std::make_shared<Data::User>(request);
                        })
                        ->initialize(adminHandler->router())
                        .shared_from_this();
    adminHandler->defaultHandler(Http::NullHandler);
    adminHandler->finish_init();
    return adminHandler;
}
std::shared_ptr<SimpleWebServer> makeSignupHandler()
{
    auto signupHandler = std::make_shared<SimpleWebServer>();
    Signup::SignupController::initialize("/signup", signupHandler->router());
    signupHandler->defaultHandler(Http::NullHandler);
    signupHandler->finish_init();
    return signupHandler;
}
void makeProfile(Http::Router& router)
{
    auto profileController = std::make_shared<Login::ProfileController>();
    profileController->initialize(router);
}
std::shared_ptr<LoginController> makeLoginController(Http::Router& router)
{
    auto handler = std::make_shared<SimpleWebServer>();
    PasswordChangeController::initialize("/password", handler->router());
    makeCruds(handler->router());
    makeHome(handler->router());
    makeProfile(handler->router());
    Document::DocumentController::initialize(handler->router());
    handler->defaultHandler(Http::NotFoundHandler);
    handler->finish_init();

    shared_ptr<SimpleWebServer> adminHandler = nullptr;
#ifdef ENABLE_USER_LIST
    adminHandler = makeAdminHandler();
#endif
    shared_ptr<SimpleWebServer> publicHandler = nullptr;
#ifdef ENABLE_SIGNUP
    publicHandler = makeSignupHandler();
#endif
    auto presentation = std::make_shared<Presentation>();

    auto server = std::make_shared<LoginController>(
                      handler, adminHandler, publicHandler, presentation)
                      ->initialize(router)
                      .shared_from_this();
    return server;
}

int LoginServerApplication::main(const vector<string>& args)
{
    PocoWebServer httpServer;
    auto presentation = std::make_shared<Presentation>();

    auto loginController = makeLoginController(httpServer.router());
    httpServer.defaultHandler(loginController->getDefaultHandler());
    httpServer.setPresentation(presentation);
    httpServer.finish_init();

    httpServer.start();
    // Start a "Fake" Webmail server, for testing password restore via email
    // process?
    waitForTerminationRequest();
    httpServer.stop();
    return EXIT_OK;
}
