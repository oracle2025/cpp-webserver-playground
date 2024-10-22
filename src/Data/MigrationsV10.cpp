#include "MigrationsV10.hpp"

#include "Migration.hpp"

#include <Poco/Data/Session.h>
#include <Poco/Data/Statement.h>

extern Poco::Data::Session* g_session;

namespace Data {
void MigrationsV10::perform()
{
    MigrationsV9::perform();
    Migration migration;
    if (migration.version() >= 10) {
        return;
    }
    using namespace Poco::Data::Keywords;
    using Poco::Data::Statement;

    *g_session << R"(ALTER TABLE time_events
ADD COLUMN note VARCHAR DEFAULT '';
)",
        now;

    migration.version(10);
}
} // namespace Data