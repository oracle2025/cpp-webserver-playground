#include "MigrationsV7.hpp"

#include "Migration.hpp"

#include <Poco/Data/Session.h>
#include <Poco/Data/Statement.h>

extern Poco::Data::Session* g_session;

namespace Data {
void MigrationsV7::perform()
{
    MigrationsV6::perform();
    Migration migration;
    if (migration.version() >= 7) {
        return;
    }
    using namespace Poco::Data::Keywords;
    using Poco::Data::Statement;

    *g_session << "CREATE TABLE Session ("
                  "id VARCHAR PRIMARY KEY, "
                  "isLoggedIn INTEGER(3),"
                  "userId VARCHAR, "
                  "userName VARCHAR, "
                  "createdAt VARCHAR, "
                  "lastUsedAt VARCHAR, "
                  "path VARCHAR, "
                  "userAgent VARCHAR, "
                  "hasAlert INTEGER(3), "
                  "alert VARCHAR, "
                  "alertType VARCHAR, "
                  "FOREIGN KEY (userId) REFERENCES User(id) "
                  ")",
        now;

    migration.version(7);
}
} // namespace Data