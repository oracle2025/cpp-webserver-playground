#include "Data/MigrationsV1.hpp"
#include "Login/LoginController.hpp"
#include "Signup/SignupController.hpp"
#include "TestServer.hpp"
#include "doctest.h"

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
        SignupController<TestServer> w("/signup");
        map<string, string> params;
        params["username"] = "porky";
        params["password"] = "S3cr3t!";
        params["confirm_password"] = "S3cr3t!";
        auto response = w.handle({"/signup/submit", {}, params});
        CHECK(response->alert().message() == "User created");
    }
    SUBCASE("Signup and Login")
    {
        Router router;
        auto handler = std::make_shared<SimpleWebServer>();
        ;
        CrudController<Todo>("/todo", handler->router());
        LoginController<TestServer> w(
            handler,
            nullptr,
            make_shared<SignupController<SimpleWebServer>>("/signup"),
            std::make_shared<Html::Presentation>());
        CHECK(w.handle({"/secret"})->content() == "Access denied");
        auto response = w.handle({"/signup/"});
        CHECK((response->content().find("username") != string::npos));
        auto cookieJar = response->cookies();
        map<string, string> params;
        params["username"] = "porky";
        params["password"] = "S3cr3t!";
        params["confirm_password"] = "S3cr3t!";
        response = w.handle({"/signup/submit", cookieJar, params});
        CHECK(response->alert().message() == "User created");
        cookieJar.merge(response->cookies());
        CHECK(w.handle({"/secret", cookieJar})->content() == "Success");
    }
}