#include "Data/MigrationsV1.hpp"
#include "Data/Todo.hpp"
#include "Login/LoginController.hpp"
#include "NullHandler.hpp"
#include "Server/CrudController.hpp"
#include "Signup/SignupController.hpp"
#include "Impl/SimpleWebServer.hpp"
#include "doctest.h"
#include "Html/Presentation.hpp"

#include <Poco/Data/SQLite/Connector.h>

using Signup::SignupController;

TEST_CASE("Signup")
{
    Poco::Data::SQLite::Connector::registerConnector();
    Poco::Data::Session session("SQLite", ":memory:");
    g_session = &session;

    Data::MigrationsV1 m;
    m.perform();

    SUBCASE("Signup")
    {
        SimpleWebServer w;
        SignupController signup_controller("/signup", w.router());
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

        CrudController crud(
            "/todo",
            [](const Request& request) {
                return std::make_shared<Todo>(request);
            },
            handler->router());
        handler->defaultHandler(Http::NullHandler);
        handler->finish_init();

        auto signupHandler = std::make_shared<SimpleWebServer>();
        SignupController signup_controller(
            "/signup", signupHandler->router());
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