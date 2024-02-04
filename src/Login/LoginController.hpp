#pragma once

#include "Data/User.hpp"
#include "Form.hpp"
#include "Http/Request.hpp"
#include "Http/Response.hpp"
#include "Http/Session.hpp"
#include "Impl/SimpleWebServer.hpp"
#include "List.hpp"
#include "Password.hpp"
#include "Presentation.hpp"
#include "Server/CrudController.hpp"
#include "Server/TestServer.hpp"
#include "Server/style.hpp"
#include "Submit.hpp"
#include "Text.hpp"
#include "doctest.h"

#include <map>
#include <memory>
#include <set>
#include <string>
#include <utility>
using Http::redirect;
using std::map;
using std::set;
using std::string;

struct LoginController {
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

    handler_type getDefaultHandler()
    {
        return [this](const Request& request) {
            if (m_adminHandler && Http::Session(request).isLoggedIn()
                && Http::Session(request).isAdmin()) {
                auto response = m_adminHandler->handle(request);
                if (response) {
                    return addLinksToResponse(request, response);
                } else {
                    return forwardToSecretHandler(request);
                }
            } else if (Http::Session(request).isLoggedIn()) {
                return forwardToSecretHandler(request);
            } else if (!m_publicHandler) {
            } else if (auto response = m_publicHandler->handle(request)) {
                return response->appendNavBarAction({"🔒 Login", "/", "right"})
                    .shared_from_this();
            }
            return redirect("/")
                ->alert("Please login", Html::AlertType::INFO)
                .shared_from_this();
        };
    }
    LoginController(
        shared_ptr<RequestHandler> secretHandler,
        shared_ptr<RequestHandler> adminHandler,
        shared_ptr<RequestHandler> publicHandler,
        shared_ptr<Html::Presentation> presentation, Http::Router& router)
        : m_secretHandler(std::move(secretHandler))
        , m_adminHandler(std::move(adminHandler))
        , m_publicHandler(std::move(publicHandler))
        , m_presentation(std::move(presentation))
    {
        using Http::Session;
        router.get("/", [this](const Request& request) {
            if (Session(request).isLoggedIn() && m_secretHandler) {
                return forwardToSecretHandler(request);
            } else {
                return redirect("/login");
            }
        });
        router.get(
            "/login", [this](const Request& request) { return loginForm(); });
        router.post("/login", [this](const Request& request) {
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
            Http::Session::addAlertToSession(request, *response);
            return response;
        });
        router.get("/secret", [](const Request& request) {
            if (Session(request).isLoggedIn()) {
                return content("Success");
            } else {
                return content("Access denied")
                    ->code(Response::UNAUTHORIZED)
                    .shared_from_this();
            }
        });
        router.get("/logout", [this](const Request& request) {
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
        router.get("/sessions", [this](const Request& request) {
            if (Session(request).isAdmin()) {
                auto response = content(Html::List(
                                            Session::listAll(),
                                            {"id",
                                             "user_id",
                                             "is_logged_in",
                                             "createdAt",
                                             "lastUsedAt",
                                             "path",
                                             "userAgent"})
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
        router.get("/css/style.css", [](const Request& request) {
            return content(STYLE_SHEET, "text/css");
        });
        //T::defaultHandler(getDefaultHandler());
        //T::setPresentation(m_presentation);
        //T::finish_init();
    }
    shared_ptr<Response> forwardToSecretHandler(const Request& request)
    {
        auto response
            = addLinksToResponse(request, m_secretHandler->handle(request));
        Http::Session::addAlertToSession(request, *response);
        return response;
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
        /*
         * 1) Pass ElementPtr instead of string to constructor
         * 2) Retrieve From from response during unit test:
         * auto form = handle("/get_form").form();
         * form.input("field", "value")
         * handle(form.submit())
         */
        auto text = std::make_shared<Form>(
            vector<ElementPtr>{
                make_shared<Text>("username"),
                make_shared<Password>("password"),
                make_shared<Submit>("Login")},
            "/login",
            "post");
        return content((*text)())
            ->form(text)
            .title("Login")
#ifdef ENABLE_SIGNUP
            .appendNavBarAction({"Signup", "/signup/", "right"})
#endif
            .shared_from_this();
    }
};
