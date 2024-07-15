#include "Data/Migrations.hpp"
#include "LoginServerApplication.hpp"
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
        const auto connectionString = "todo.sqlite";
#else
        const auto connectionString = TODO_DATABASE_DIR "/todo.sqlite";
#endif
        Session session("SQLite", connectionString);
        g_session = &session;
        Data::MigrationsLatest migration;
        migration.perform();
        spdlog::info("Config Directory: {}", CONFIG_DIR);
        spdlog::info("Database Directory: {}", TODO_DATABASE_DIR);
        spdlog::info("Template Directory: {}", TEMPLATE_DIR);
        LoginServerApplication app;
        return app.run(argc, argv);

    } catch (...) {
        std::ostringstream str;
        Trace::backtrace(std::current_exception(), str);
        spdlog::error("Exception: {}", str.str());

        return EXIT_FAILURE;
    }
}