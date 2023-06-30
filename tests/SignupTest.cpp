#include "Data/MigrationsV1.hpp"
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

    SignupController<TestServer> w("/signup");

    map<string, string> params;
    params["username"] = "porky";
    params["password"] = "S3cr3t!";
    params["confirm_password"] = "S3cr3t!";
    auto response = w.handle({"/signup/submit", {}, params});
    CHECK(response->alert().message() == "User created");
}