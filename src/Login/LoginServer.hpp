#pragma once

#include "Form.hpp"
#include "Http/Request.hpp"
#include "Http/Response.hpp"
#include "Password.hpp"
#include "Presentation.hpp"
#include "Server/CrudServer.hpp"
#include "Server/RecursiveWebServer.hpp"
#include "Server/TestServer.hpp"
#include "Server/style.hpp"
#include "SessionId.hpp"
#include "Submit.hpp"
#include "Text.hpp"
#include "doctest.h"

#include <map>
#include <set>
#include <string>
using std::map;
using std::set;
using std::string;

template<typename T>
struct LoginServer : public T {
    static bool isLoginAttempt(const map<string, string>& parameters)
    {
        return parameters.count("username") && parameters.count("password");
    }

    static bool isValidUser(const map<string, string>& parameters)
    {
        const auto username = "admin";
        const auto password = "Adm1n!";
        return (parameters.at("username") == username)
            && (parameters.at("password") == password);
    }

    bool hasValidSession(const Request& request) const
    {
        return request.hasCookie("session-id")
            && m_sessions.count(SessionId{request.cookie("session-id")});
    }

    void clearSession(const Request& request)
    {
        if (request.hasCookie("session-id")) {
            m_sessions.erase(SessionId{request.cookie("session-id")});
        }
    }

    LoginServer(
        shared_ptr<RequestHandler> secretHandler,
        shared_ptr<Presentation> presentation)
        : m_secretHandler(secretHandler)
        , m_presentation(presentation)
    {
        T::get("/", [this](const Request& request) {
            if (hasValidSession(request) && m_secretHandler) {
                return m_secretHandler->handle(request);
            } else {
                return loginForm();
            }
        });
        T::post("/login", [this](const Request& request) {
            if (!isLoginAttempt(request.allParameters())) {
                return content("Invalid Request");
            }
            if (!isValidUser(request.allParameters())) {
                setMessage("Invalid Login");
                return loginForm();
            }
            auto sessionId = generateRandomSessionId();
            m_sessions.insert(sessionId);
            return redirect("/")
                ->content("Success")
                .cookie("session-id", sessionId)
                .shared_from_this();
        });
        T::get("/secret", [this](const Request& request) {
            if (hasValidSession(request)) {
                return content("Success");
            } else {
                return content("Access denied");
            }
        });
        T::get("/logout", [this](const Request& request) {
            if (hasValidSession(request)) {
                clearSession(request);
                return redirect("/")
                    ->cookie("session-id", "")
                    .alert("Logged out")
                    .shared_from_this();
            } else {
                return content("Access denied");
            }
        });
        T::get("/css/style.css", [](const Request& request) {
            return content(STYLE_SHEET, "text/css");
        });
        T::defaultHandler([this](const Request& request) {
            if (hasValidSession(request)) {
                return m_secretHandler->handle(request);
            } else {
                return content("Access denied")
                    ->code(Response::UNAUTHORIZED)
                    .shared_from_this();
            }
        });
        T::setPresentation(m_presentation);
        T::finish_init();
    }
    void setMessage(const string& message)
    {
        m_message = message;
    }
    string showMessage()
    {
        if (m_message.empty()) {
            return "";
        }
        auto result = m_message;
        m_message.clear();
        return R"(<div class="alert-danger">⚠️ )" + result + R"(</div>)";
    }

private:
    set<SessionId> m_sessions;
    shared_ptr<RequestHandler> m_secretHandler;
    shared_ptr<Presentation> m_presentation;
    string m_message;

    shared_ptr<Response> loginForm()
    {
        using namespace Input;
        auto text = Form(
                        {Text("username")(),
                         Password("password")(),
                         Submit("submit")()},
                        "/login",
                        "post")();
        return content(text)
            ->title("Login")
            .alert(m_message)
            .shared_from_this();
    }
};

TEST_CASE("Show Message")
{
    LoginServer<TestServer> w(nullptr, nullptr);
    w.setMessage("Hello");
    CHECK_FALSE(w.showMessage().empty());
    CHECK(w.showMessage().empty());
}
