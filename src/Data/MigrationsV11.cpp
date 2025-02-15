
#include "MigrationsV11.hpp"

#include "Migration.hpp"

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

    // List all CSV files in data/ folder
    // Insert into calendar_entries according to correct category

    migration.version(11);
}
} // namespace Data