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
        shared_ptr<Html::Presentation> presentation)
        : m_secretHandler(secretHandler)
        , m_presentation(presentation)
    {
        T::router().get("/", [this](const Request& request) {
            if (hasValidSession(request) && m_secretHandler) {
                return forwardToSecretHandler(request);
            } else {
                return loginForm();
            }
        });
        T::router().get("/login", [this](const Request& request) {
            if (!isLoginAttempt(request.allParameters())) {
                return content("Invalid Request");
            }
            if (!isValidUser(request.allParameters())) {
                return redirect("/")->alert("Invalid Login", Html::AlertType::DANGER)
                    .shared_from_this();
            }
            auto sessionId = generateRandomSessionId();
            m_sessions.insert(sessionId);
            return redirect("/")
                ->alert("Logged in successfully", Html::AlertType::SUCCESS)
                .cookie("session-id", sessionId)
                .shared_from_this();
        });
        T::router().get("/secret", [this](const Request& request) {
            if (hasValidSession(request)) {
                return content("Success");
            } else {
                return content("Access denied");
            }
        });
        T::router().get("/logout", [this](const Request& request) {
            if (hasValidSession(request)) {
                clearSession(request);
                return redirect("/")
                    ->cookie("session-id", "")
                    .alert("Logged out", Html::AlertType::INFO)
                    .shared_from_this();
            } else {
                return content("Access denied");
            }
        });
        T::router().get("/css/style.css", [](const Request& request) {
            return content(STYLE_SHEET, "text/css");
        });
        T::defaultHandler([this](const Request& request) {
            if (hasValidSession(request)) {
                return forwardToSecretHandler(request);
            } else {
                return content("Access denied")
                    ->code(Response::UNAUTHORIZED)
                    .shared_from_this();
            }
        });
        T::setPresentation(m_presentation);
        T::finish_init();
    }
    shared_ptr<Response> forwardToSecretHandler(const Request& request)
    {
        return m_secretHandler->handle(request)
            ->appendNavBarAction({"🚪 Logout", "/logout", "right"})
            .shared_from_this();
        ;
    }

private:
    set<SessionId> m_sessions;
    shared_ptr<RequestHandler> m_secretHandler;
    shared_ptr<Html::Presentation> m_presentation;

    shared_ptr<Response> loginForm()
    {
        using namespace Input;
        auto text = Form(
            {Text("username")(), Password("password")(), Submit("Login")()},
            "/login",
            "post")();
        return content(text)
            ->title("Login")
            .shared_from_this();
    }
};

