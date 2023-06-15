#include "Data/MigrationsV0.hpp"
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
    using MigrationsV0 = Data::MigrationsV0;
    try {
        Poco::Data::SQLite::Connector::registerConnector();
        Session session("SQLite", ":memory:");
        g_session = &session;
        MigrationsV0 migration;
        migration.perform();

        /*Todo todo = Todo::RecordType{"0123", "Buy Milk", "", "", 0};
        todo.create_table();
        todo.insert();
        todo = Todo::RecordType{"0123", "Empty Trash", "", "", 0};
        todo.insert();*/

        /*User user;
        user.create_table();
        user.username = "admin";
        user.setPassword("Adm1n!"); // generates random salt
        user.insert();*/

        LoginServerApplication app;
        return app.run(argc, argv);
    } catch (Poco::Exception& exc) {
        cerr << exc.displayText() << endl;
        return EXIT_FAILURE;
    }
}