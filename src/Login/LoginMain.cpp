#include "Data/MigrationsV1.hpp"
#include "LoginServerApplication.hpp"
#include "spdlog/spdlog.h"
#include "Trace/trace.hpp"

#include <Poco/Data/SQLite/Connector.h>
#include <Poco/Data/Session.h>


extern Poco::Data::Session* g_session;

using Poco::Data::Session;
int main(int argc, char** argv)
{
    using MigrationsV1 = Data::MigrationsV1;
    try {
        Poco::Data::SQLite::Connector::registerConnector();
        Session session("SQLite", "todo.sqlite");
        const auto connectionString = TODO_DATABASE_DIR "/todo.sqlite";
        //Session session("SQLite", connectionString);
        g_session = &session;
        MigrationsV1 migration;
        migration.perform();
        LoginServerApplication app;
        return app.run(argc, argv);

    } catch (...) {
        std::ostringstream str;
        Trace::backtrace(std::current_exception(), str);
        spdlog::error("Exception: {}", str.str());

        return EXIT_FAILURE;
    }
}