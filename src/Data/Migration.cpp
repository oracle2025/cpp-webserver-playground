
#include "Migration.hpp"

#include "doctest.h"

#include <Poco/Data/SQLite/Connector.h>
#include <Poco/Data/Session.h>

extern Poco::Data::Session* g_session;

namespace Data {

Migration::Migration()
{
    using namespace Poco::Data::Keywords;
    using Poco::Data::Statement;
    *g_session
        << "CREATE TABLE IF NOT EXISTS migrations (version INTEGER NOT NULL);",
        now;
}

bool Migration::hasVersion()
{
    using namespace Poco::Data::Keywords;
    using Poco::Data::Statement;
    Statement select(*g_session);
    int count;
    select << "SELECT COUNT(*) FROM migrations;", into(count), now;
    if (count == 0) {
        return false;
    } else if (count > 1) {
        throw std::runtime_error("Too many migrations rows");
    }
    return true;
}

int Migration::version()
{
    using namespace Poco::Data::Keywords;
    using Poco::Data::Statement;
    Statement select(*g_session);
    int result = -1;
    select << "SELECT version FROM migrations;", into(result), range(0, 1), now;
    return result;
}
void Migration::version(int version)
{
    using namespace Poco::Data::Keywords;
    using Poco::Data::Statement;
    if (!hasVersion()) {
        Statement insert(*g_session);
        insert << "INSERT INTO migrations (version) VALUES (?);", use(version),
            now;
        return;
    }
    Statement update(*g_session);
    update << "UPDATE migrations SET version = ?;", use(version), now;
}
bool Migration::isDatabaseAtVersion(int version)
{
    if (!hasVersion()) {
        return false;
    }
    return this->version() == version;
}

} // namespace Data

TEST_CASE("Migration")
{
    using Session = Poco::Data::Session;
    Poco::Data::SQLite::Connector::registerConnector();
    Session session("SQLite", ":memory:");
    g_session = &session;
    SUBCASE("empty database")
    {
        Data::Migration migration;
        CHECK(migration.isDatabaseAtVersion(0) == false);
    }
    SUBCASE("initial migration")
    {
        Data::Migration migration;
        migration.version(0);
        CHECK(migration.isDatabaseAtVersion(0) == true);
    }
    SUBCASE("second migration")
    {
        Data::Migration migration;
        migration.version(0);
        CHECK(migration.isDatabaseAtVersion(0) == true);
        migration.version(1);
        CHECK(migration.isDatabaseAtVersion(0) == false);
        CHECK(migration.isDatabaseAtVersion(1) == true);
    }
}