#include "TestServer.hpp"
#include "Todo.hpp"
#include "doctest.h"
#include "CrudServer.hpp"

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
    CrudServer<TestServer> w;
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
        auto response = w.getResponse("/edit?invalid-uuid");
        CHECK_EQ(response->code(), 404);
    }
}