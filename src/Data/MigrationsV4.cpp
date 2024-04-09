
#include "MigrationsV4.hpp"

#include "Migration.hpp"

#include <Poco/Data/Session.h>
#include <Poco/Data/Statement.h>

extern Poco::Data::Session* g_session;

namespace Data {
void MigrationsV4::perform()
{
    MigrationsV3::perform();
    Migration migration;
    if (migration.version() >= 4) {
        return;
    }
    using namespace Poco::Data::Keywords;
    using Poco::Data::Statement;
    *g_session
        << "ALTER TABLE Users ADD COLUMN start_page VARCHAR DEFAULT '/' ",
        now;

    migration.version(4);
}
} // namespace Data