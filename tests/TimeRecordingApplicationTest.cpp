#include "Data/Migrations.hpp"
#include "Data/RecordImpl.hpp"
#include "Html/Presentation.hpp"
#include "Impl/SimpleWebServer.hpp"
#include "Login/LoginController.hpp"
#include "TimeRecordingApp/TimeRecordingApplication.hpp"
#include "doctest.h"
#include "PocoPageHandler.hpp"
#include "FakeBrowser.hpp"

#include <Poco/Data/SQLite/Connector.h>
#include <Poco/Data/Session.h>

void login(FakeBrowser& browser)
{
    browser.location("http://localhost:8080/login");
    CHECK(browser.form().get() != nullptr);
    browser.form()->set("username", "user");
    browser.form()->set("password", "S3cr3t&");
    browser.submit();
    CHECK(
        browser.pageContents().find("Logged in successfully") != std::string::npos);
}

TEST_CASE("TimeRecordingApplicationTest")
{
    Poco::Data::SQLite::Connector::registerConnector();
    Poco::Data::Session session("SQLite", ":memory:");
    g_session = &session;
    Data::MigrationsLatest m;
    m.perform();
    Data::User user;
    user.username = "user";
    user.setPassword("S3cr3t&");
    user.set("role", "user");
    user.insert();

    SimpleWebServer handler;
    auto presentation = std::make_shared<Html::Presentation>("Time Recording");

    auto login_controller = TimeRecordingApplication::makeLoginController(
        handler.router(), presentation);
    handler.defaultHandler(login_controller->getDefaultHandler());
    handler.setPresentation(presentation);
    handler.finish_init();
    PocoPageHandler pageHandler(
        [&handler](const Request& request) { return handler.handle(request); },
        presentation);
    FakeBrowser browser(pageHandler);
    login(browser);

    SUBCASE("Create Time Entry")
    {
        browser.location("/list/");
        CHECK(browser.pageContents().find("Noch keine Einträge") != std::string::npos);
        browser.location("http://localhost:8080/time_entry/");
        CHECK(browser.form() != nullptr);
        browser.form()->set("event_time", "11:00");
        browser.form()->set("event_type", "start");
        browser.submit();
        CHECK(browser.pageContents().find("Time Entry created") != std::string::npos);
        browser.location("/list/");
        CHECK(browser.pageContents().find("11:00 -") != string::npos);
    }
    SUBCASE("Edit Time Entry")
    {
        CHECK(browser.form() != nullptr);
        browser.form()->set("event_time", "11:00");
        browser.form()->set("event_type", "start");
        browser.submit();
        CHECK(browser.pageContents().find("Time Entry created") != std::string::npos);
        browser.location("/list/");
        auto all_links = browser.extractLinks(browser.pageContents());
        auto edit_link_position = browser.pageContents().find("href=\"/list/edit?") + 6;
        CHECK_NE(edit_link_position, std::string::npos);
        auto edit_link = browser.pageContents().substr(edit_link_position);
        auto link_end = edit_link.find("\"");
        auto link_location = edit_link.substr(0, link_end);
        //CHECK_EQ(link_location.substr(20), "/list/edit?");
        //CHECK_EQ(link_location.size(), 37);
        // TODO
    }
    SUBCASE("Test extractLinks")
    {
        std::string html = "<a href=\"/list/edit?7f74fe03-d834-4d5e-bde7-58712c755781\">Edit</a>";
        auto links = browser.extractLinks(html);
        CHECK_EQ(links.size(), 1);
        CHECK_EQ(std::get<0>(links[0]), "Edit");
        CHECK_EQ(std::get<1>(links[0]), "/list/edit?7f74fe03-d834-4d5e-bde7-58712c755781");
    }
    SUBCASE("Extending URIs")
    {
        Poco::URI uri1("/read?7f74fe03-d834-4d5e-bde7-58712c755781");
        CHECK_EQ(uri1.getPath(), "/read");
        CHECK_EQ(uri1.getScheme(), "");
        CHECK_EQ(uri1.getUserInfo(), "");
        CHECK_EQ(uri1.getHost(), "");
        CHECK_EQ(uri1.getQuery(), "7f74fe03-d834-4d5e-bde7-58712c755781");
        CHECK_EQ(uri1.getFragment(), "");
        CHECK_EQ(uri1.getSpecifiedPort(), 0);
        CHECK_EQ(uri1.toString(), "/read?7f74fe03-d834-4d5e-bde7-58712c755781");
    }
}