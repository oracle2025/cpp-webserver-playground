#include "Html/Presentation.hpp"
#include "Http/Request.hpp"
#include "Http/Response.hpp"
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

TEST_CASE("Alert after redirect")
{
    SimpleAlertComponent<TestServer> w;
    auto r = w.handle({"/redirect_and_alert"});
    auto cookieJar = r->cookies();
    Request request = {r->location(), cookieJar};
    r = w.handle(request);
    auto page = Html::Presentation().render(*r);
//    CHECK(page.find("This is an Alert") != string::npos);
}

TEST_CASE("Alert after multiple redirects")
{

}