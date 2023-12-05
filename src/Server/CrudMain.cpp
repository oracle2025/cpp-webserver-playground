#include "CrudServerApplication.hpp"

#include <iostream>
using std::cerr;
using std::endl;
#include "Data/Todo.hpp"

#include <Poco/Data/SQLite/Connector.h>
#include <Poco/Data/Session.h>
using Poco::Data::Session;

int main()
{
    try {
        Poco::Data::SQLite::Connector::registerConnector();
        Session session("SQLite", ":memory:");
        g_session = &session;
        Todo todo{Todo::RecordType{"0123", "Buy Milk", "", "", false}};
        todo.create_table();
        todo.insert();
        Todo todo2{Todo::RecordType{"0123", "Empty Trash", "", "", false}};
        todo2.insert();
        CrudServerApplication app;
        static char buffer[MAX_INPUT];
        strncpy(buffer, "app", MAX_INPUT);
        char* argv[] = {buffer};
        app.run(1, argv);
    } catch (Poco::Exception& exc) {
        cerr << exc.displayText() << endl;
    }
    return 0;
}