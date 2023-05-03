#include "Data/Todo.hpp"
#include "LoginServerApplication.hpp"

#include <iostream>
using std::cerr;
using std::endl;

#include <Poco/Data/SQLite/Connector.h>
#include <Poco/Data/Session.h>
using Poco::Data::Session;
int main(int argc, char** argv)
{
    try {
        Poco::Data::SQLite::Connector::registerConnector();
        Session session("SQLite", ":memory:");
        g_session = &session;
        Todo::create_table();
        Todo todo = {"0123", "Buy Milk", "", "", 0};
        todo.insert();
        todo = {"0123", "Empty Trash", "", "", 0};
        todo.insert();
        LoginServerApplication app;
        return app.run(argc, argv);
    } catch (Poco::Exception& exc) {
        cerr << exc.displayText() << endl;
        return EXIT_FAILURE;
    }
}