#include "doctest.h"

#include <Poco/Data/SQLite/Connector.h>
#include <Poco/Data/Session.h>
using Poco::Data::Session;
#include "Data/Migrations.hpp"
#include "Data/Todo.hpp"
#include "Filter/ByOwner.hpp"
#include "SimpleWebServer.hpp"
#include "TodoController.hpp"

#include <memory>

void insert_todos()
{
    for (int i = 0; i < 20; i++) {
        auto todo = std::make_shared<Todo>(Todo::RecordType{
            "0123", "Buy Milk", time_string(), time_string(), false, "", ""});
        todo->insert();
    }
}

TEST_CASE("Todo Performance")
{
    Poco::Data::SQLite::Connector::registerConnector();
    Session session("SQLite", ":memory:");
    g_session = &session;

    Data::MigrationsLatest m;
    m.perform();
    auto handler = std::make_shared<SimpleWebServer>();

    auto todoCrud = std::make_shared<TodoController>(
                        "/todo",
                        [](const Http::Request& request) {
                            return std::make_shared<Todo>(request);
                        })
                        ->initialize(handler->router())
                        .shared_from_this();
    insert_todos();

    // Save starting time
    auto start = std::chrono::high_resolution_clock::now();
    auto response = handler->handle({"/todo/"});
    // Save ending time
    auto end = std::chrono::high_resolution_clock::now();
    // Calculate duration
    auto duration
        = std::chrono::duration_cast<std::chrono::milliseconds>(end - start);
    // Output duration
    std::cout << "Duration: " << duration.count() << "ms" << std::endl;
}