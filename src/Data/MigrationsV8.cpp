#include "MigrationsV8.hpp"

#include "Migration.hpp"

#include <Poco/Data/Session.h>
#include <Poco/Data/Statement.h>

extern Poco::Data::Session* g_session;

namespace Data {
void MigrationsV8::perform()
{
    MigrationsV7::perform();
    Migration migration;
    if (migration.version() >= 8) {
        return;
    }
    using namespace Poco::Data::Keywords;
    using Poco::Data::Statement;

    *g_session << R"(CREATE TABLE time_events (
id VARCHAR,
employee_id VARCHAR NOT NULL,
event_date DATE NOT NULL,
event_time TIME NOT NULL,
event_type VARCHAR CHECK( event_type IN ('start', 'stop', 'correction', 'deletion') ) NOT NULL DEFAULT 'start',
corrected_event_id VARCHAR,  -- References the event_id of the entry it corrects (if event_type = 'correction')
deleted_event_id VARCHAR,    -- References the event_id of the entry it deletes (if event_type = 'deletion')
creation_date DATETIME NOT NULL,
creator_user_id VARCHAR NOT NULL);
)",
        now;

    migration.version(8);
}
} // namespace Data