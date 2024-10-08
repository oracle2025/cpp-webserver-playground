#include "MigrationsV9.hpp"

#include "Migration.hpp"

#include <Poco/Data/Session.h>
#include <Poco/Data/Statement.h>
extern Poco::Data::Session* g_session;
namespace Data {
void MigrationsV9::perform()
{
    MigrationsV8::perform();
    Migration migration;
    if (migration.version() >= 9) {
        return;
    }
    using namespace Poco::Data::Keywords;
    using Poco::Data::Statement;

    *g_session
        << R"(ALTER TABLE Users
ADD COLUMN role VARCHAR NOT NULL DEFAULT 'user';)",
        now;

    migration.version(9);
}
} // namespace Data