#include "MigrationsV11.hpp"

#include "Migration.hpp"
#include "String/createRandomUUID.hpp"

#include <Poco/Data/Session.h>
#include <Poco/Data/Statement.h>

extern Poco::Data::Session* g_session;

namespace Data {
void MigrationsV11::perform()
{
    MigrationsV10::perform();
    Migration migration;
    if (migration.version() >= 11) {
        return;
    }
    using namespace Poco::Data::Keywords;
    using Poco::Data::Statement;

    *g_session << R"(CREATE TABLE calendar_categories(
        id VARCHAR,
        name VARCHAR
);
)",now;

    // Add api_key column to Users table (SQLite supports ADD COLUMN)
        *g_session << "ALTER TABLE Users ADD COLUMN api_key VARCHAR", now;


    // For all existing users, generate an API key (UUID) and store it
        Statement select(*g_session);
        std::string userId;
        select << "SELECT id FROM Users", into(userId), range(0, 1);
        while (!select.done()) {
            if (select.execute()) {
                std::string apiKey = String::createRandomUUID();
                Statement update(*g_session);
                update << "UPDATE Users SET api_key = ? WHERE id = ?",
                    use(apiKey), use(userId), now;
            }
        }

    migration.version(11);
}
} // namespace Data