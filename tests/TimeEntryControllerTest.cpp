#include "Data/Migrations.hpp"
#include "FakeBrowser.hpp"
#include "Html/Presentation.hpp"
#include "Impl/SimpleWebServer.hpp"
#include "Login/LoginController.hpp"
#include "PocoPageHandler.hpp"
#include "TimeRecordingApp/TimeCorrectionController.hpp"
#include "TimeRecordingApp/TimeEntryController.hpp"
#include "doctest.h"

#include <Poco/Data/SQLite/Connector.h>

TEST_CASE("TimeEntryControllerTest")
{
    Poco::Data::SQLite::Connector::registerConnector();
    Poco::Data::Session session("SQLite", ":memory:");
    g_session = &session;
    Data::MigrationsLatest m;
    m.perform();
    SimpleWebServer handler;
    auto timeEntryHandler = std::make_shared<SimpleWebServer>();
    auto timeEntryController
        = std::make_shared<TimeEntryController>("/time_entry");
    timeEntryController->initialize(timeEntryHandler->router());
    timeEntryHandler->router().get("/", [](const Request& request) {
        return Http::redirect("/time_entry/");
    });
    auto timeCorrectionController
        = std::make_shared<TimeCorrectionController>("/list");
    timeCorrectionController->initialize(timeEntryHandler->router());
    auto presentation = std::make_shared<Html::Presentation>("Time Recording");

    auto login_controller = std::make_shared<LoginController>(
                                timeEntryHandler, nullptr, nullptr, nullptr)
                                ->initialize(handler.router())
                                .shared_from_this();
    handler.defaultHandler(login_controller->getDefaultHandler());
    handler.setPresentation(presentation);
    handler.finish_init();
    PocoPageHandler pageHandler(
        [&handler](const Request& request) { return handler.handle(request); },
        presentation);
    FakeBrowser browser(pageHandler);
    SUBCASE("Create Time Entry")
    {
        browser.location("http://localhost:8080/login");
        CHECK(browser.form().get() != nullptr);
        browser.form()->set("username", "admin");
        browser.form()->set("password", "Adm1n!");
        browser.submit();
        CHECK(
            browser.pageContents().find("Logged in successfully")
            != string::npos);
        browser.location("/list/");
        CHECK(
            browser.pageContents().find("Noch keine Einträge") != string::npos);
        browser.location("http://localhost:8080/time_entry/");
        CHECK(browser.form() != nullptr);
        browser.form()->set("event_time", "11:00");
        browser.form()->set("event_type", "start");
        browser.submit();
        CHECK(
            browser.pageContents().find("Time Entry created") != string::npos);
        browser.location("/list/");
        CHECK(browser.pageContents().find("11:00 -") != string::npos);
    }
    SUBCASE("Create Entry with Note at Stop")
    {
        browser.location("http://localhost:8080/login");
        CHECK(browser.form().get() != nullptr);
        browser.form()->set("username", "admin");
        browser.form()->set("password", "Adm1n!");
        browser.submit();
        CHECK(
            browser.pageContents().find("Logged in successfully")
            != string::npos);
        browser.location("http://localhost:8080/time_entry/");
        CHECK(browser.form() != nullptr);
        browser.form()->set("event_time", "11:00");
        browser.form()->set("event_type", "start");
        browser.submit();
        CHECK(
            browser.pageContents().find("Time Entry created") != string::npos);
        browser.location("http://localhost:8080/time_entry/");
        CHECK(browser.form() != nullptr);
        browser.form()->set("event_time", "12:00");
        browser.form()->set("event_type", "stop");
        browser.form()->set("note", "NoteCanary");
        browser.submit();
        CHECK(
            browser.pageContents().find("Time Entry created") != string::npos);
        browser.location("/list/");
        CHECK(browser.pageContents().find("11:00 - 12:00") != string::npos);
        CHECK(browser.pageContents().find("NoteCanary") != string::npos);
    }
    SUBCASE("Create Entry with Note at Start")
    {
        browser.location("http://localhost:8080/login");
        CHECK(browser.form().get() != nullptr);
        browser.form()->set("username", "admin");
        browser.form()->set("password", "Adm1n!");
        browser.submit();
        CHECK(
            browser.pageContents().find("Logged in successfully")
            != string::npos);
        browser.location("http://localhost:8080/time_entry/");
        CHECK(browser.form() != nullptr);
        browser.form()->set("event_time", "11:00");
        browser.form()->set("event_type", "start");
        browser.form()->set("note", "NoteCanary");
        browser.submit();
        CHECK(
            browser.pageContents().find("Time Entry created") != string::npos);
        CHECK(browser.pageContents().find("NoteCanary") != string::npos);

        browser.location("/list/");
        CHECK(browser.pageContents().find("NoteCanary") != string::npos);


        browser.location("http://localhost:8080/time_entry/");
        CHECK(browser.form() != nullptr);
        browser.form()->set("event_time", "12:00");
        browser.form()->set("event_type", "stop");
        browser.submit();
        CHECK(
            browser.pageContents().find("Time Entry created") != string::npos);
        browser.location("/list/");
        CHECK(browser.pageContents().find("NoteCanary") != string::npos);
    }
}