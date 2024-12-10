#include "Data/Migrations.hpp"
#include "FakeBrowser.hpp"
#include "Http/Session.hpp"
#include "Impl/SimpleWebServer.hpp"
#include "Login/LoginController.hpp"
#include "PocoPageHandler.hpp"
#include "TimeRecordingApp/TimeCorrectionController.hpp"
#include "TimeRecordingApp/TimeEntry.hpp"
#include "doctest.h"
#include "spdlog/spdlog.h"

#include <Poco/Data/SQLite/Connector.h>
static void insert_for_start(
    TimeEntry& t, const std::string& day, const std::string& start)
{
    t.set("event_date", day);
    t.set("event_time", start);
    t.set("event_type", "start");
    t.insert();
}

static map<string, string> login(RequestHandler& w)
{
    map<string, string> params;
    params["username"] = "admin";
    params["password"] = "Adm1n!";
    auto response
        = w.handle({"/login", {}, params, "", Http::Method::POST});
    auto cookieJar = response->cookies();
    return cookieJar;
}

TEST_CASE("TimeCorrectionController")
{
    spdlog::set_level(spdlog::level::debug);
    using Http::Session;
    Poco::Data::SQLite::Connector::registerConnector();
    Poco::Data::Session session("SQLite", ":memory:");
    g_session = &session;

    Data::MigrationsLatest m;
    m.perform();

    SimpleWebServer w;
    auto timeCorrectionServer = std::make_shared<SimpleWebServer>();
    auto timeCorrectionController
        = std::make_shared<TimeCorrectionController>("/list")
              ->initialize(timeCorrectionServer->router())
              .shared_from_this();
    auto presentation = std::make_shared<Html::Presentation>("Time Recording");

    auto login_controller
        = std::make_shared<LoginController>(
              timeCorrectionServer, nullptr, nullptr, nullptr)
              ->initialize(w.router())
              .shared_from_this();
    w.defaultHandler(login_controller->getDefaultHandler());
    w.setPresentation(presentation);
    w.finish_init();
    PocoPageHandler pageHandler(
        [&w](const Request& request) { return w.handle(request); },
        presentation);
    FakeBrowser browser(pageHandler);
    SUBCASE("List Empty") {
        CHECK(w.handle({"/list/"})->status() == 302);
        map<string, string> params;
        params["username"] = "admin";
        params["password"] = "Adm1n!";
        auto response
            = w.handle({"/login", {}, params, "", Http::Method::POST});
        auto cookieJar = response->cookies();
        auto actual = w.handle({"/list/", cookieJar, {}})->content();
        CHECK(actual.find("Noch keine Einträge") != string::npos);
    }
    SUBCASE("List Single Entry") {
        Data::User u;
        Data::findUser(*g_session, "admin", u);
        TimeEntry t;
        t.set("employee_id", u.key());
        insert_for_start(t, "2024-03-18", "23:55");
        map<string, string> params;
        params["username"] = "admin";
        params["password"] = "Adm1n!";
        auto response
            = w.handle({"/login", {}, params, "", Http::Method::POST});
        auto cookieJar = response->cookies();
        auto actual = w.handle({"/list/", cookieJar, {}})->content();
        CHECK(actual.find("18. März") != string::npos);
        CHECK(actual.find("23:55") != string::npos);
    }
    SUBCASE("Create Entry") {
        auto cookieJar = login(w);
        auto response_new_entry_form = w.handle({"/list/new", cookieJar});
        const auto content = response_new_entry_form->content();
        CHECK(content.find("Create Entry") != string::npos);
        response_new_entry_form->form();
    }
}