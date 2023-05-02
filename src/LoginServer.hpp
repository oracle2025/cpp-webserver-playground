#pragma once

#include "CrudServer.hpp"
#include "Form.hpp"
#include "Password.hpp"
#include "RecursiveWebServer.hpp"
#include "Request.hpp"
#include "Response.hpp"
#include "SessionId.hpp"
#include "Submit.hpp"
#include "TestServer.hpp"
#include "Text.hpp"
#include "doctest.h"
#include "style.hpp"

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

    LoginServer(shared_ptr<RequestHandler> secretHandler)
        : m_secretHandler(secretHandler)
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

                /*
                 * CrudServer crud;
                 * return crud.handle(request);
                 */

                return content("Success");
            } else {
                return content("Access denied");
            }
        });
        T::get("/logout", [this](const Request& request) {
            if (hasValidSession(request)) {
                clearSession(request);
                return content("Logged out")
                    ->cookie("session-id", "")
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
        const string header = R"(<!doctype html><html lang="de"><head>
<meta charset="utf-8">
<meta name="viewport" content="width=device-width, initial-scale=1.0">
<link rel="stylesheet" type="text/css" href="css/style.css">
</head>
<body>
<div class="container">
<h2>Login Form</h2>
)";
        const string footer = R"(</div></body></html>)";
        auto result = content(header + showMessage() + text + footer);
        return result;
    }
};

TEST_CASE("Show Message") {
    LoginServer<TestServer> w(nullptr);
    w.setMessage("Hello");
    CHECK_FALSE(w.showMessage().empty());
    CHECK(w.showMessage().empty());
}
