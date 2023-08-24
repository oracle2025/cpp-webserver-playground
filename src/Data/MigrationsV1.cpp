#include "MigrationsV1.hpp"

#include "Migration.hpp"
#include "String/createRandomUUID.hpp"

#include <Poco/Data/SQLite/Connector.h>
#include <Poco/Data/Session.h>

extern Poco::Data::Session* g_session;

namespace Data {

void MigrationsV1::perform()
{
    MigrationsV0::perform();
    Migration migration;
    if (migration.version() >= 1) {
        return;
    }
    /*User user;
    user.create_table();
    user.username = "admin";
    user.setPassword("Adm1n!");
    user.insert();*/
    using namespace Poco::Data::Keywords;
    using std::string;
    auto createStatement
        = "CREATE TABLE Users (id VARCHAR, username VARCHAR, password "
          "VARCHAR, salt VARCHAR)";
    *g_session << createStatement, now;
    auto insertStatement = "INSERT INTO Users (id, username, password, salt) "
                           "VALUES (?, ?, ?, ?)";
    auto salt = String::createRandomUUID();
    Poco::Tuple<string, string, Poco::Data::CLOB, string> values{
        String::createRandomUUID(), "admin", "Adm1n!" + salt, salt};
    Poco::Data::Statement insert(*g_session);
    insert << insertStatement, use(values), now;
    migration.version(1);
}
} // namespace Data
