
#include "MigrationsV5.hpp"

#include "Migration.hpp"

#include <Poco/Data/Session.h>
#include <Poco/Data/Statement.h>

extern Poco::Data::Session* g_session;

namespace Data {
void MigrationsV5::perform()
{
    MigrationsV4::perform();
    Migration migration;
    if (migration.version() >= 5) {
        return;
    }
    using namespace Poco::Data::Keywords;
    using Poco::Data::Statement;
    *g_session
        << "ALTER TABLE Todo ADD COLUMN parent_id VARCHAR DEFAULT '' ",
        now;

    migration.version(5);
}
} // namespace Data