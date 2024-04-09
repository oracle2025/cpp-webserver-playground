

#include "MigrationsV2.hpp"

#include "Data/V2/UserV2.hpp"
#include "Migration.hpp"
#include "MigrationsV1.hpp"
#include "Trace/trace.hpp"
#include "User.hpp"

#include <Poco/Data/Session.h>

extern Poco::Data::Session* g_session;
namespace Data {
void MigrationsV2::perform()
{
    MigrationsV1::perform();
    Migration migration;
    if (migration.version() >= 2) {
        return;
    }
    try {
        using namespace Poco::Data::Keywords;
        // Can't use this here, because the User may be changed, wold need something like UserV2, ... that stays put
        V2::UserV2 user;
        auto users = user.list();
        auto user_id  = users.front().key();
        // Hardcoding the table creation is not a good idea, because if
        // the migrations runs from scratch, it will already create the user_id
        // column, and then the migration will fail.
        // if table todo has column user_id, then return

        *g_session << "ALTER TABLE todo ADD COLUMN user_id VARCHAR DEFAULT '" + user_id + "'", now;
    } catch (...) {
        TRACE_RETHROW("Could not update table");
    }
    migration.version(2);
}
} // namespace Data