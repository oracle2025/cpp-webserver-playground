#include "Data/MigrationsV3.hpp"
#include "FakeBrowser.hpp"
#include "Login/LoginController.hpp"
#include "Server/TestServer.hpp"
#include "User/PasswordChangeController.hpp"
#include "doctest.h"

#include <Poco/Data/SQLite/Connector.h>

bool loginSuccess(const string& user, const string& password, RequestHandler& w)
{
    map<string, string> params;
    params["username"] = user;
    params["password"] = password;
    auto response = w.handle({"/login", {}, params, "", Http::Method::POST});
    auto cookieJar = response->cookies();
    while (response->status() == 302) {
        response = w.handle({response->location(), cookieJar});
        cookieJar.merge(response->cookies());
    }
    return response->title().find("Change Password") != string::npos;
}

TEST_CASE("Change Password")
{
    Poco::Data::SQLite::Connector::registerConnector();
    Poco::Data::Session session("SQLite", ":memory:");
    g_session = &session;

    Data::MigrationsLatest m;
    m.perform();

    Data::User user;

    TestServer w;
    auto passwordChangeHandler = std::make_shared<SimpleWebServer>();
    PasswordChangeController password_change_controller(
        "/password", passwordChangeHandler->router());

    LoginController login_controller(
        passwordChangeHandler, nullptr, nullptr, nullptr, w.router());
    w.defaultHandler(login_controller.getDefaultHandler());
    w.setPresentation(nullptr);
    w.finish_init();
    SUBCASE("Change password")
    {
        CHECK(loginSuccess("admin", "Adm1n!", w));
        CHECK(w.handle({"/password/"})->status() == 302);
        map<string, string> params;
        params["username"] = "admin";
        params["password"] = "Adm1n!";
        auto response
            = w.handle({"/login", {}, params, "", Http::Method::POST});
        auto cookieJar = response->cookies();
        auto actual = w.handle({"/password/", cookieJar, {}})->content();
        CHECK(actual.find("current_password") != string::npos);
        CHECK(actual.find("new_password") != string::npos);
        CHECK(actual.find("confirm_password") != string::npos);
        params.clear();
        params["current_password"] = "Adm1n!";
        params["new_password"] = "S3cr3t&";
        params["confirm_password"] = "S3cr3t&";

        response = w.handle(
            {"/password/update", cookieJar, params, "", Http::Method::POST});
        CHECK(response->content() == "Password updated successfully");
        CHECK(loginSuccess("admin", "S3cr3t&", w));
    }
    /*
     * 1) Login with existing password
     * 2) Open Form for Password change
     * 3) submit changed password
     * 4) logout
     * 5) login with new password
     */

    // Given: a user with a password
    // When: the user logs in
    // Then: the user is logged in

    // Given: a user with a password
    // When: the user changes the password
    // Then: the user is logged in with the new password

    // Given: a logged in user
    // When: the user changes the password
    // Then: the user can log in with the new password
}

TEST_CASE("Change Password with Fake Browser")
{
    Poco::Data::SQLite::Connector::registerConnector();
    Poco::Data::Session session("SQLite", ":memory:");
    g_session = &session;
    Data::MigrationsLatest m;
    m.perform();
    TestServer handler;
    auto passwordChangeHandler = std::make_shared<SimpleWebServer>();
    PasswordChangeController password_change_controller(
        "/password", passwordChangeHandler->router());
    LoginController login_controller(
        passwordChangeHandler,
        nullptr,
        nullptr,
        nullptr,
        handler.router());
    handler.defaultHandler(login_controller.getDefaultHandler());
    handler.setPresentation(nullptr);
    handler.finish_init();
    PocoPageHandler pageHandler(
        [&handler](const Request& request) { return handler.handle(request); },
        nullptr);
    FakeBrowser browser(pageHandler);
    browser.location("http://localhost:8080/login");
    CHECK(browser.form().get() != nullptr);
    browser.form()->set("username", "admin");
    browser.form()->set("password", "Adm1n!");
    browser.submit();
    CHECK(
        browser.pageContents().find("Logged in successfully") != string::npos);
    browser.location("http://localhost:8080/password/");
    browser.form()->set("current_password", "Adm1n!");
    browser.form()->set("new_password", "S3cr3t&");
    browser.form()->set("confirm_password", "S3cr3t&");
    browser.submit();
    CHECK(
        browser.pageContents().find("Password updated successfully")
        != string::npos);
    browser.location("http://localhost:8080/logout");
    browser.location("http://localhost:8080/login");
    CHECK(browser.form().get() != nullptr);
    browser.form()->set("username", "admin");
    browser.form()->set("password", "S3cr3t&");
    browser.submit();
    CHECK(
        browser.pageContents().find("Logged in successfully") != string::npos);
}