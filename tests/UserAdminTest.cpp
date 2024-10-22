#include "Data/Migrations.hpp"
#include "Data/User.hpp"
#include "FakeBrowser.hpp"
#include "Html/Presentation.hpp"
#include "Http/NullHandler.hpp"
#include "Http/NotFoundHandler.hpp"
#include "Http/Session.hpp"
#include "Impl/SimpleWebServer.hpp"
#include "Login/LoginController.hpp"
#include "PocoPageHandler.hpp"
#include "User/UserAdminController.hpp"
#include "doctest.h"

#include <Poco/Data/SQLite/Connector.h>

void loginAsAdmin(FakeBrowser& browser)
{
    browser.location("http://localhost:8080/login");
    CHECK(browser.form().get() != nullptr);
    browser.form()->set("username", "admin");
    browser.form()->set("password", "Adm1n!");
    browser.submit();
    CHECK(
        browser.pageContents().find("Logged in successfully") != string::npos);
}

TEST_CASE("UserAdminTest")
{
    using Http::Session;
    Poco::Data::SQLite::Connector::registerConnector();
    Poco::Data::Session session("SQLite", ":memory:");
    g_session = &session;

    Data::MigrationsLatest m;
    m.perform();

    auto adminHandler = make_shared<SimpleWebServer>();
    auto userCrud = std::make_shared<UserAdminController>(
                        "/user",
                        [](const Request& request) {
                            return std::make_shared<Data::User>(request);
                        })
                        ->initialize(adminHandler->router())
                        .shared_from_this();
    adminHandler->defaultHandler(Http::NullHandler);
    adminHandler->finish_init();

    auto userHandler  = make_shared<SimpleWebServer>();
    userHandler->defaultHandler(Http::NotFoundHandler);
    userHandler->finish_init();

    SimpleWebServer w;
    auto login_controller = std::make_shared<LoginController>(
                                userHandler, adminHandler, nullptr, nullptr)
                                ->initialize(w.router())
                                .shared_from_this();
    w.defaultHandler(login_controller->getDefaultHandler());
    auto presentation = std::make_shared<Html::Presentation>("Todo List");
    w.setPresentation(presentation);
    w.finish_init();
    Session::clearAll();
    PocoPageHandler pageHandler(
        [&w](const Request& request) { return w.handle(request); },
        presentation);
    FakeBrowser browser(pageHandler);
    SUBCASE("Create User and Login as User")
    {
        loginAsAdmin(browser);
        browser.location("http://localhost:8080/user/new");
        browser.form()->set("username", "tiger");
        browser.form()->set("password", "S3cr3t&");
        browser.form()->set("confirm_password", "S3cr3t&");
        browser.form()->set("role", "user");
        browser.submit();
        CHECK(browser.pageContents().find("User created") != string::npos);
        browser.location("http://localhost:8080/logout");
        browser.location("http://localhost:8080/login");
        CHECK(browser.form().get() != nullptr);
        browser.form()->set("username", "tiger");
        browser.form()->set("password", "S3cr3t&");
        browser.submit();
        CHECK(
            browser.pageContents().find("Logged in successfully")
            != string::npos);
    }
    SUBCASE("Change User Role and Login as User")
    {
        loginAsAdmin(browser);
        browser.location("http://localhost:8080/user/new");
        browser.form()->set("username", "tiger");
        browser.form()->set("password", "S3cr3t&");
        browser.form()->set("confirm_password", "S3cr3t&");
        browser.form()->set("role", "user");
        browser.submit();
        CHECK(browser.pageContents().find("User created") != string::npos);
        browser.location("http://localhost:8080/user/");
        CHECK(browser.find_and_follow("Edit",1));
        CHECK(browser.form().get() != nullptr);
        CHECK(browser.form()->data().find("tiger") != std::string::npos);
        browser.form()->set("role", "newrole");
        browser.submit();
        CHECK(browser.pageContents().find("User updated") != string::npos);
        browser.location("http://localhost:8080/user/");
        CHECK(browser.pageContents().find("newrole") != string::npos);
        browser.location("http://localhost:8080/logout");
        browser.location("http://localhost:8080/login");
        CHECK(browser.form().get() != nullptr);
        browser.form()->set("username", "tiger");
        browser.form()->set("password", "S3cr3t&");
        browser.submit();
        CHECK(
            browser.pageContents().find("Logged in successfully")
            != string::npos);
    }
    SUBCASE("Reset User Password and Login as User")
    {

    }
}