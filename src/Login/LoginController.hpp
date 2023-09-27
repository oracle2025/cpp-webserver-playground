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

    static bool isValidUser(
        const map<string, string>& parameters, Data::User& user)
    {
        return user.isValidUser(
            parameters.at("username"), parameters.at("password"), user);
    }

    LoginController(
        shared_ptr<RequestHandler> secretHandler,
        shared_ptr<RequestHandler> adminHandler,
        shared_ptr<RequestHandler> publicHandler,
        shared_ptr<Html::Presentation> presentation)
        : m_secretHandler(secretHandler)
        , m_adminHandler(adminHandler)
        , m_publicHandler(publicHandler)
        , m_presentation(presentation)
    {
        using Http::Session;
        T::router().get("/", [this](const Request& request) {
            if (Session(request).isLoggedIn() && m_secretHandler) {
                return forwardToSecretHandler(request, m_secretHandler);
            } else {
                return redirect("/login");
            }
        });
        T::router().get(
            "/login", [this](const Request& request) {
                return loginForm();
            });
        T::router().post("/login", [this](const Request& request) {
            if (!isLoginAttempt(request.allParameters())) {
                return content("Invalid Request");
            }
            Data::User user;
            if (!isValidUser(request.allParameters(), user)) {
                return loginForm()
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
        T::router().get("/secret", [](const Request& request) {
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
                auto response = loginForm()
                                    ->alert("Logged out", Html::AlertType::INFO)
                                    .shared_from_this();
                Session(request).current(*response).logout();
                Session(request).clearSession();
                return response;
            } else {
                return content("Access denied")
                    ->code(Response::UNAUTHORIZED)
                    .shared_from_this();
            }
        });
        T::router().get("/sessions", [this](const Request& request) {
            if (Session(request).isAdmin()) {
                auto response = content(Html::List(
                                            Session::listAll(),
                                            {"id",
                                             "user_id",
                                             "is_logged_in",
                                             "createdAt",
                                             "lastUsedAt",
                                            "path","userAgent"})
                                            .withHeader()())
                                    ->appendNavBarAction({"Start", "/"})
                                    .shared_from_this();
                return addLinksToResponse(request, response);
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
            if (m_adminHandler && Session(request).isLoggedIn()
                && Session(request).isAdmin()) {
                auto response = m_adminHandler->handle(request);
                if (response) {
                    return addLinksToResponse(request, response);
                } else {
                    return forwardToSecretHandler(request, m_secretHandler);
                }
            } else if (Session(request).isLoggedIn()) {
                return forwardToSecretHandler(request, m_secretHandler);
            } else if (!m_publicHandler) {
            } else if (auto response = m_publicHandler->handle(request)) {
                return response->appendNavBarAction({"🔒 Login", "/", "right"})
                    .shared_from_this();
            }
            return redirect("/")
                ->alert("Please login", Html::AlertType::INFO)
                .shared_from_this();
        });
        T::setPresentation(m_presentation);
        T::finish_init();
    }
    shared_ptr<Response> forwardToSecretHandler(
        const Request& request, shared_ptr<RequestHandler> handler)
    {
        return addLinksToResponse(request, handler->handle(request));
        ;
    }
    shared_ptr<Response> addLinksToResponse(
        const Request& request, shared_ptr<Response> response) const
    {
        using Http::Session;
        if (Session(request).isAdmin()) {
#ifdef ENABLE_USER_LIST
            response->appendNavBarAction({"Users", "/user/", "right"});
#endif
            response->appendNavBarAction({"Sessions", "/sessions", "right"});
        }
        return response->appendNavBarAction({"🚪 Logout", "/logout", "right"})
            .appendNavBarAction(
                {"👤 " + Session(request).userName(), "/password/", "right"})
            .shared_from_this();
    }

private:
    shared_ptr<RequestHandler> m_secretHandler;
    shared_ptr<RequestHandler> m_adminHandler;
    shared_ptr<RequestHandler> m_publicHandler;
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
#ifdef ENABLE_SIGNUP
            .appendNavBarAction({"Signup", "/signup/", "right"})
#endif
            .shared_from_this();
    }
};
