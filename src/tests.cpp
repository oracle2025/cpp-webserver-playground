#include "CrudServerApplication.hpp"
#include "Todo.hpp"
#include "doctest.h"

#include <Poco/Data/SQLite/Connector.h>
#include <Poco/Data/Session.h>
#include <Poco/UUIDGenerator.h>

#include <functional>
#include <iostream>

using namespace std;
using Poco::Data::Session;

TEST_CASE("Poco Web Server")
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
        CrudServerApplication app;
        static char buffer[MAX_INPUT];
        strncpy(buffer, "app", MAX_INPUT);
        char* argv[] = {buffer};
        app.run(1, argv);
    } catch (Poco::Exception& exc) {
        cerr << exc.displayText() << endl;
    }
}