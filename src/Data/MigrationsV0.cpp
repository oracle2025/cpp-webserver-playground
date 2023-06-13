#include "MigrationsV0.hpp"

#include "Migration.hpp"
#include "Todo.hpp"
#include "doctest.h"

#include <Poco/Data/SQLite/Connector.h>

namespace Data {
void MigrationsV0::perform()
{
    Migration migration;
    if (migration.version() == 0) {
        return; // Return if the database is already at version 0
    }
    Todo todo;
    todo.create_table();
    migration.version(0);
}
} // namespace Data

TEST_CASE("MigrationsV0")
{
    using Session = Poco::Data::Session;
    Poco::Data::SQLite::Connector::registerConnector();
    Session session("SQLite", ":memory:");
    g_session = &session;
    SUBCASE("initial migration")
    {
        Data::MigrationsV0 migrationsV0;
        migrationsV0.perform();
    }
    SUBCASE("noop migration")
    {

    }
}