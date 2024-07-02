#include "Data/Migrations.hpp"
#include "TimeRecordingApplication.hpp"
#include "Trace/trace.hpp"
#include "spdlog/spdlog.h"

#include <Poco/Data/SQLite/Connector.h>
#include <Poco/Data/Session.h>

extern Poco::Data::Session* g_session;

using Poco::Data::Session;
int main(int argc, char** argv)
{
    try {
        Poco::Data::SQLite::Connector::registerConnector();
#ifdef DEBUG_BUILD
        const auto connectionString = "time_recording.sqlite";
#else
        const auto connectionString = TIME_RECORDING_DATABASE_DIR "/time_recording.sqlite";
#endif
        Session session("SQLite", connectionString);
        g_session = &session;
        Data::MigrationsLatest migration;
        migration.perform();
        spdlog::info("Config Directory: {}", CONFIG_DIR);
        spdlog::info("Database Directory: {}", TIME_RECORDING_DATABASE_DIR);
        spdlog::info("Template Directory: {}", TEMPLATE_DIR);
        spdlog::info("Connection String: {}", connectionString);
        TimeRecordingApplication app;
        return app.run(argc, argv);

    } catch (...) {
        std::ostringstream str;
        Trace::backtrace(std::current_exception(), str);
        spdlog::error("Exception: {}", str.str());

        return EXIT_FAILURE;
    }
}