#pragma once

#include "Data/User.hpp"
#include "Form.hpp"
#include "Http/Request.hpp"
#include "Http/Response.hpp"
#include "Http/Session.hpp"
#include "Impl/SimpleWebServer.hpp"
#include "Password.hpp"
#include "Presentation.hpp"
#include "Server/CrudController.hpp"
#include "Server/TestServer.hpp"
#include "Server/style.hpp"
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
struct LoginController : public T {
    static bool isLoginAttempt(const map<string, string>& parameters)
    {
        return parameters.count("username") && parameters.count("password");
    }

    static bool isValidUser(const map<string, string>& parameters, User& user)
    {
        return user.isValidUser(
            parameters.at("username"), parameters.at("password"), user);
    }

    LoginController(
        shared_ptr<RequestHandler> secretHandler,
        shared_ptr<Html::Presentation> presentation)
        : m_secretHandler(secretHandler)
        , m_presentation(presentation)
    {
        using Http::Session;
        T::router().get("/", [this](const Request& request) {
            if (Session(request).isLoggedIn() && m_secretHandler) {
                return forwardToSecretHandler(request);
            } else {
                return loginForm();
            }
        });
        T::router().get("/login", [this](const Request& request) {
            if (!isLoginAttempt(request.allParameters())) {
                return content("Invalid Request");
            }
            User user;
            if (!isValidUser(request.allParameters(), user)) {
                return redirect("/")
                    ->alert("Invalid Login", Html::AlertType::DANGER)
                    .shared_from_this();
            }
            auto response
                = redirect("/")
                      ->alert(
                          "Logged in successfully", Html::AlertType::SUCCESS)
                      .shared_from_this();
            Session(request).clearSession();
            Session(request).current(*response).login(user);
            return response;
        });
        T::router().get("/secret", [this](const Request& request) {
            if (Session(request).isLoggedIn()) {
                return content("Success");
            } else {
                return content("Access denied")
                    ->code(Response::UNAUTHORIZED)
                    .shared_from_this();
            }
        });
        T::router().get("/logout", [this](const Request& request) {
            if (Session(request).isLoggedIn()) {
                auto response = redirect("/")
                                    ->alert("Logged out", Html::AlertType::INFO)
                                    .shared_from_this();
                Session(request).current(*response).logout();
                return response;
            } else {
                return content("Access denied")
                    ->code(Response::UNAUTHORIZED)
                    .shared_from_this();
            }
        });
        T::router().get("/sessions", [this](const Request& request) {
            if (Session(request).isLoggedIn()) {
                return content(Html::List(
                    Session::listAll(), {"id", "user_id", "is_logged_in"})());
            } else {
                return content("Access denied")
                    ->code(Response::UNAUTHORIZED)
                    .shared_from_this();
            }
        });
        T::router().get("/css/style.css", [](const Request& request) {
            return content(STYLE_SHEET, "text/css");
        });
        T::defaultHandler([this](const Request& request) {
            if (Session(request).isLoggedIn()) {
                return forwardToSecretHandler(request);
            } else {
                return redirect("/")
                    ->alert("Please login", Html::AlertType::INFO)
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
            .appendNavBarAction({"👤 Account", "/password/", "right"})
            .shared_from_this();
        ;
    }

private:
    shared_ptr<RequestHandler> m_secretHandler;
    shared_ptr<Html::Presentation> m_presentation;

    shared_ptr<Response> loginForm()
    {
        using namespace Input;
        auto text = Form(
            {Text("username")(), Password("password")(), Submit("Login")()},
            "/login",
            "post")();
        return content(text)->title("Login").shared_from_this();
    }
};
