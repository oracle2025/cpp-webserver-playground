#include "Data/MigrationsV1.hpp"
#include "Data/Todo.hpp"
#include "Data/User.hpp"
#include "LoginServerApplication.hpp"

#include <iostream>
using std::cerr;
using std::endl;

#include <Poco/Data/SQLite/Connector.h>
#include <Poco/Data/Session.h>
using Poco::Data::Session;
int main(int argc, char** argv)
{
    using MigrationsV1 = Data::MigrationsV1;
    try {
        Poco::Data::SQLite::Connector::registerConnector();
        Session session("SQLite", ":memory:");
        g_session = &session;
        MigrationsV1 migration;
        migration.perform();



        LoginServerApplication app;
        return app.run(argc, argv);
    } catch (Poco::Exception& exc) {
        cerr << exc.displayText() << endl;
        return EXIT_FAILURE;
    }
}