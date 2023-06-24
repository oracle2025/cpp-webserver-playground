#include "Data/MigrationsV1.hpp"
#include "Login/LoginController.hpp"
#include "Server/TestServer.hpp"
#include "User/PasswordChangeComponent.hpp"
#include "doctest.h"

#include <Poco/Data/SQLite/Connector.h>

bool loginSuccess(const string& user, const string& password, RequestHandler& w)
{
        map<string, string> params;
        params["username"] = user;
        params["password"] = password;
        auto response = w.handle({"/login", {}, params});
        auto cookieJar = response->cookies();
        while(response->status() == 302) {
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

    Data::MigrationsV1 m;
    m.perform();

    User user;

    LoginController<TestServer> w(
        std::make_shared<PasswordChangeComponent<TestServer>>("/password"),
        nullptr);
    SUBCASE("Change password")
    {
        CHECK(loginSuccess("admin", "Adm1n!", w));
        CHECK(w.handle({"/password/"})->status() == 302);
        map<string, string> params;
        params["username"] = "admin";
        params["password"] = "Adm1n!";
        auto response = w.handle({"/login", {}, params});
        auto cookieJar = response->cookies();
        auto actual
            = w.handle({"/password/", cookieJar, {}})->content();
        CHECK(actual.find("current_password") != string::npos);
        CHECK(actual.find("new_password") != string::npos);
        CHECK(actual.find("confirm_password") != string::npos);
        params.clear();
        params["current_password"] = "Adm1n!";
        params["new_password"] = "S3cr3t&";
        params["confirm_password"] = "S3cr3t&";

        response = w.handle({"/password/update", cookieJar, params});
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