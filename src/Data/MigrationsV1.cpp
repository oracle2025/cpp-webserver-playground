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

TEST_CASE("MigrationV1")
{
    using Session = Poco::Data::Session;
    Poco::Data::SQLite::Connector::registerConnector();
    Session session("SQLite", ":memory:");
    g_session = &session;

    Data::MigrationsV1 m;
    m.perform();

    User user;
    for (auto &u:user.list()) {
        CHECK(u.username == "admin");
        CHECK(u.password == "Adm1n!" + u.salt);
    }
}