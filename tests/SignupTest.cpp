#include "Data/Migrations.hpp"
#include "Data/MigrationsV1.hpp"
#include "Data/Todo.hpp"
#include "Html/Presentation.hpp"
#include "Impl/SimpleWebServer.hpp"
#include "Login/LoginController.hpp"
#include "NullHandler.hpp"
#include "Server/CrudController.hpp"
#include "Signup/SignupController.hpp"
#include "doctest.h"

#include <Poco/Data/SQLite/Connector.h>

using Signup::SignupController;

TEST_CASE("Signup")
{
    Poco::Data::SQLite::Connector::registerConnector();
    Poco::Data::Session session("SQLite", ":memory:");
    g_session = &session;

    Data::MigrationsLatest m;
    m.perform();

    SUBCASE("Signup")
    {
        SimpleWebServer w;
        SignupController::initialize("/signup", w.router());
        map<string, string> params;
        params["username"] = "porky";
        params["password"] = "S3cr3t!";
        params["confirm_password"] = "S3cr3t!";
        auto response
            = w.handle({"/signup/submit", {}, params, {}, Http::Method::POST});
        CHECK(response->alert().message() == "User created");
    }
    SUBCASE("Signup and Login")
    {
        auto handler = std::make_shared<SimpleWebServer>();

        auto crud = std::make_shared<CrudController>(
                        "/todo",
                        [](const Request& request) {
                            return std::make_shared<Todo>(request);
                        })
                        ->initialize(handler->router())
                        .shared_from_this();
        handler->defaultHandler(Http::NullHandler);
        handler->finish_init();

        auto signupHandler = std::make_shared<SimpleWebServer>();
        SignupController::initialize("/signup", signupHandler->router());
        signupHandler->defaultHandler(Http::NullHandler);
        signupHandler->finish_init();

        SimpleWebServer w;
        auto presentation = std::make_shared<Html::Presentation>();
        LoginController login_controller(
            handler, nullptr, signupHandler, presentation, w.router());
        w.defaultHandler(login_controller.getDefaultHandler());
        w.setPresentation(presentation);
        w.finish_init();
        CHECK(w.handle({"/secret"})->content() == "Access denied");
        auto response = w.handle({"/signup/"});
        CHECK((response->content().find("username") != string::npos));
        auto cookieJar = response->cookies();
        map<string, string> params;
        params["username"] = "porky";
        params["password"] = "S3cr3t!";
        params["confirm_password"] = "S3cr3t!";
        response = w.handle(
            {"/signup/submit", cookieJar, params, {}, Http::Method::POST});
        CHECK(response->alert().message() == "User created");
        cookieJar.merge(response->cookies());
        CHECK(w.handle({"/secret", cookieJar})->content() == "Success");
    }
}