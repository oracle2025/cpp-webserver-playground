#include "MigrationsV3.hpp"

#include "Migration.hpp"
#include "PasswordSalting.hpp"
#include "User.hpp"
#include "doctest.h"

#include <Poco/Data/SQLite/Connector.h>
#include <Poco/Data/Session.h>

extern Poco::Data::Session* g_session;

namespace Data {
void MigrationsV3::perform()
{
    MigrationsV2::perform();
    Migration migration;
    if (migration.version() >= 3) {
        return;
    }
    using namespace Poco::Data::Keywords;
    using Poco::Data::Statement;
    using std::string;
    using std::vector;
    auto alterStatement
        = "ALTER TABLE Users ADD COLUMN hash BLOB AFTER password";
    *g_session << alterStatement, now;
    using valueType
        = Poco::Tuple<string, string, string, Poco::Data::CLOB, string>;
    vector<valueType> values;
    Statement select(*g_session);
    select << "SELECT id, username, password, hash, salt FROM Users",
        into(values), now;
    for (auto& value : values) {
        string id = value.get<0>();
        string password = value.get<2>();
        string salt = value.get<4>();
        Poco::Data::CLOB hash;
        password = password.substr(0, password.size() - salt.size());
        hash = Data::PasswordSalting(password, salt).hash();
        auto updateStatement = "UPDATE Users SET hash = ? WHERE id = ?";
        Statement update(*g_session);
        update << updateStatement, use(hash), use(id), now;
    }
    *g_session << "ALTER TABLE Users DROP COLUMN password", now;
    *g_session << "ALTER TABLE Users RENAME hash TO password", now;
    migration.version(3);
}
} // namespace Data
TEST_CASE("MigrationV3PasswordSalting")
{
    using Session = Poco::Data::Session;
    Poco::Data::SQLite::Connector::registerConnector();
    Session session("SQLite", ":memory:");
    g_session = &session;

    Data::MigrationsV3 m;
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
TEST_CASE("MigrationV3")
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