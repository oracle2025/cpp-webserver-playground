#include "MigrationsV0.hpp"

#include "Migration.hpp"
#include "Todo.hpp"
#include "doctest.h"

#include <Poco/Data/SQLite/Connector.h>
extern Poco::Data::Session* g_session;
namespace Data {
void MigrationsV0::perform()
{
    Migration migration;
    if (migration.hasVersion()) {
        return;
    }
    using namespace Poco::Data::Keywords;
    auto createStatement
        = "CREATE TABLE Todo (id VARCHAR, description VARCHAR, created_at "
          "VARCHAR, updated_at VARCHAR, checked INTEGER(3))";
    *g_session << createStatement, now;
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