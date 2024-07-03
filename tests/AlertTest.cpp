#include "Data/Migrations.hpp"
#include "Html/Presentation.hpp"
#include "Http/NotFoundHandler.hpp"
#include "Http/Request.hpp"
#include "Http/Response.hpp"
#include "Http/Session.hpp"
#include "Impl/SimpleWebServer.hpp"
#include "doctest.h"

#include <Poco/Data/SQLite/Connector.h>

using Http::content;
using Http::redirect;

template<typename T>
struct SimpleAlertComponent : public T {
    using Request = Http::Request;
    SimpleAlertComponent()
    {
        T::router().get("/alert", [](const Request& request) {
            return content("Hello World")
                ->alert("This is an Alert", Html::AlertType::SUCCESS)
                .shared_from_this();
        });
        T::router().get("/redirect_and_alert", [](const Request& request) {
            return redirect("/hello")
                ->alert("This is an Alert", Html::AlertType::SUCCESS)
                .shared_from_this();
        });
        T::router().get("/hello", [](const Request& request) {
            return content("Hello World");
        });
        T::router().get(
            "/alert_and_redirect_twice", [](const Request& request) {
                return redirect("/redirect")
                    ->alert("This is an Alert", Html::AlertType::SUCCESS)
                    .shared_from_this();
            });
        T::router().get("/redirect", [](const Request& request) {
            return redirect("/hello");
        });
    }
};

struct SimpleSessionServer {
    using Session = Http::Session;
    Router& router()
    {
        return m_router;
    }
    shared_ptr<Http::Response> handle(const Http::Request& request)
    {
        auto response = m_router.handle(request);
        Session session(request);
        session.current(*response);
        Session::addAlertToSession(request, *response);
        return response;
    }

private:
    Router m_router;
};

TEST_CASE("Alerts")
{
    Poco::Data::SQLite::Connector::registerConnector();
    using Poco::Data::Session;

    Session session("SQLite", ":memory:");
    g_session = &session;

    Data::MigrationsLatest m;
    m.perform();
    SUBCASE("Alert")
    {
        SimpleAlertComponent<SimpleWebServer> w;
        auto r = w.handle({"/alert"});
        auto cookieJar = r->cookies();
        auto page = Html::Presentation().render(*r);
        CHECK(page.find("This is an Alert") != string::npos);
    }

    SUBCASE("Alert after redirect")
    {
        SimpleAlertComponent<SimpleSessionServer> w;
        auto r = w.handle({"/redirect_and_alert"});
        auto cookieJar = r->cookies();
        Request request = {r->location(), cookieJar};
        r = w.handle(request);
        auto page = Html::Presentation().render(*r);
        CHECK(page.find("This is an Alert") != string::npos);
    }

    SUBCASE("Alert after multiple redirects")
    {
        SimpleAlertComponent<SimpleSessionServer> w;
        auto r = w.handle({"/alert_and_redirect_twice"});
        CHECK(r->status() == 302);
        auto cookieJar = r->cookies();
        Request request = {r->location(), cookieJar};
        r = w.handle(request);
        CHECK(r->status() == 302);
        cookieJar.merge(r->cookies());
        request = {r->location(), cookieJar};
        r = w.handle(request);
        auto page = Html::Presentation().render(*r);
        CHECK(page.find("This is an Alert") != string::npos);
    }

    SUBCASE("Session is removed after last Alert is displayed")
    {
    }
}