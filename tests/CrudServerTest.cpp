#include "Data/Todo.hpp"
#include "Server/CrudComponent.hpp"
#include "Server/TestServer.hpp"
#include "doctest.h"

#include <Poco/Data/SQLite/Connector.h>
#include <Poco/Data/Session.h>
#include <Poco/URI.h>
#include <Poco/UUID.h>

using namespace std;
using Poco::Data::Session;

/*
 * Test Cases:
 *
 * Redirect to edit after create
 * Redirect to edit after update
 * Flash Banner after Redirect
 * Confirm before delete (Javascript?)
 */

TEST_CASE("Crud Server")
{
    CrudComponent<TestServer> w;
    Poco::Data::SQLite::Connector::registerConnector();
    Session session("SQLite", ":memory:");
    g_session = &session;
    Todo::create_table();
    SUBCASE("UUID to string")
    {
        Poco::UUID id("7f74fe03-d834-4d5e-bde7-58712c755781");
        CHECK(id.toString() == "7f74fe03-d834-4d5e-bde7-58712c755781");
    }
    SUBCASE("Poco URI")
    {
        Poco::URI uri1("/read?7f74fe03-d834-4d5e-bde7-58712c755781");
        CHECK_EQ(uri1.getPath(), "/read");
        CHECK_EQ(uri1.getQuery(), "7f74fe03-d834-4d5e-bde7-58712c755781");
    }
    SUBCASE("404 Not found")
    {
        auto response = w.handle({"/edit?invalid-uuid"});
        CHECK_EQ(response->code(), 404);
    }
    SUBCASE("Create")
    {
        auto response = w.handle({"/new"});
        CHECK_EQ(response->code(), 200);
        CHECK(response->content().find("Create") != string::npos);
    }
    SUBCASE("List")
    {
        auto response = w.handle({"/"});
        CHECK(response->actions().size() == 1);
        CHECK(response->actions().begin()->url == "/new");
        CHECK(response->actions().begin()->title == "Create new Todo");
        CHECK(response->title() == "Todo List");
    }
}