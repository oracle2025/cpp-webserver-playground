#include "MigrationsV1.hpp"

#include "Migration.hpp"
#include "User.hpp"
#include "doctest.h"

#include <Poco/Data/SQLite/Connector.h>

namespace Data {

void MigrationsV1::perform()
{
    MigrationsV0::perform();
    Migration migration;
    if (migration.version() >= 1) {
        return;
    }
    User user;
    user.create_table();
    user.username = "admin";
    user.setPassword("Adm1n!");
    user.insert();
    migration.version(1);
}
} // namespace Data

TEST_CASE("MigrationV1PasswordSalting")
{
    using Session = Poco::Data::Session;
    Poco::Data::SQLite::Connector::registerConnector();
    Session session("SQLite", ":memory:");
    g_session = &session;

    Data::MigrationsV1 m;
    m.perform();

    Data::User user;
    user.username = "mary";
    user.setPassword("Mary!");
    user.insert();

    auto expected = user.password;

    CHECK(Data::findUser(session, "mary", user));

    auto actual = user.password;

    CHECK(expected == actual);

    CHECK(Data::User::isValidUser("mary", "Mary!", user));
}
TEST_CASE("MigrationV1")
{
    using Session = Poco::Data::Session;
    Poco::Data::SQLite::Connector::registerConnector();
    Session session("SQLite", ":memory:");
    g_session = &session;

    Data::MigrationsV1 m;
    m.perform();

    Data::User user;
    for (auto& u : user.list()) {
        CHECK(u.username == "admin");
        CHECK(Data::PasswordSalting("Adm1n!", u.salt).isValid(u.password));
    }
}