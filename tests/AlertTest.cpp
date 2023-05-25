#include "Html/Presentation.hpp"
#include "Http/NotFoundHandler.hpp"
#include "Http/Request.hpp"
#include "Http/Response.hpp"
#include "Http/Session.hpp"
#include "Server/TestServer.hpp"
#include "doctest.h"

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
    }
};

TEST_CASE("Alert")
{
    SimpleAlertComponent<TestServer> w;
    auto r = w.handle({"/alert"});
    auto cookieJar = r->cookies();
    auto page = Html::Presentation().render(*r);
    CHECK(page.find("This is an Alert") != string::npos);
}

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
        if (session.hasValidSession()) {
            auto current = session.current();
            if (current.hasAlert()) {
                response->alert(
                    current.getAlert().message(),
                    current.getAlert().alertType());
                current.clearAlert();
            } else if (!response->alert().message().empty()) {
                current.alert(response->alert());
            }
        } else {
            auto& current = session.createSession(*response);
            if (!response->alert().message().empty()) {
                current.alert(response->alert());
            }
        }
        return response;
    }

private:
    Router m_router;
};

TEST_CASE("Alert after redirect")
{
    SimpleAlertComponent<SimpleSessionServer> w;
    auto r = w.handle({"/redirect_and_alert"});
    auto cookieJar = r->cookies();
    Request request = {r->location(), cookieJar};
    r = w.handle(request);
    auto page = Html::Presentation().render(*r);
    CHECK(page.find("This is an Alert") != string::npos);
}

TEST_CASE("Alert after multiple redirects")
{
}