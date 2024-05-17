#include "MigrationsV6.hpp"

#include "Migration.hpp"

#include <Poco/Data/Session.h>
#include <Poco/Data/Statement.h>

extern Poco::Data::Session* g_session;

namespace Data {
void MigrationsV6::perform()
{
    MigrationsV5::perform();
    Migration migration;
    if (migration.version() >= 6) {
        return;
    }
    using namespace Poco::Data::Keywords;
    using Poco::Data::Statement;
    *g_session << "CREATE TABLE Movie ("
                  "id VARCHAR PRIMARY KEY, "
                  "title VARCHAR, "
                  "director VARCHAR, "
                  "genre VARCHAR, "
                  "year VARCHAR, "
                  "rating VARCHAR, "
                  "user_id VARCHAR, "
                  "FOREIGN KEY (user_id) REFERENCES User(id) "
                  ")",
        now;
    migration.version(6);
}
} // namespace Data