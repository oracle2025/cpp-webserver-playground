#include "LoginController.hpp"

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
#include "Server/style.hpp"
#include "Submit.hpp"
#include "Text.hpp"
#include "doctest.h"

#include <map>
#include <memory>
#include <utility>
using Http::redirect;
using std::map;

LoginController::LoginController(
    shared_ptr<RequestHandler> secretHandler,
    shared_ptr<RequestHandler> adminHandler,
    shared_ptr<RequestHandler> publicHandler,
    shared_ptr<Html::Presentation> presentation)
    : m_secretHandler(std::move(secretHandler))
    , m_adminHandler(std::move(adminHandler))
    , m_publicHandler(std::move(publicHandler))
    , m_presentation(std::move(presentation))
    , m_postProcessingHook([](const Request&, shared_ptr<Response> response) {
        return response;
    })
{
}
LoginController& LoginController::initialize(Http::Router& router)
{
    using Http::Session;
    auto ptr = shared_from_this();
    router.get("/", [ptr](const Request& request) {
        if (Session(request).isLoggedIn() && ptr->m_secretHandler) {
            return ptr->forwardToSecretHandler(request);
        } else {
            return redirect("/login");
        }
    });
    router.get(
        "/login", [ptr](const Request& request) { return ptr->loginForm(); });
    router.post("/login", [ptr](const Request& request) {
        if (!isLoginAttempt(request.allParameters())) {
            return content("Invalid Request");
        }
        Data::User user;
        if (!isValidUser(request.allParameters(), user)) {
            return ptr->loginForm()
                ->alert("Invalid Login", Html::AlertType::DANGER)
                .shared_from_this();
        }
        auto response
            = redirect("/")
                  ->alert("Logged in successfully", Html::AlertType::SUCCESS)
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
    router.get("/logout", [ptr](const Request& request) {
        if (Session(request).isLoggedIn()) {
            auto response = ptr->loginForm()
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
    router.get("/sessions", [ptr](const Request& request) {
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
            return ptr->m_postProcessingHook(request, response);
        } else {
            return content("Access denied")
                ->code(Response::UNAUTHORIZED)
                .shared_from_this();
        }
    });
    router.get("/css/style.css", [](const Request& request) {
        return content(STYLE_SHEET, "text/css");
    });
    router.get("/images/avatars/maulwurf.jpg", [](const Request& request) {
        return std::make_shared<Http::Response>()
            ->sendFile("../html/images/avatars/maulwurf.jpg")
            .mimetype("image/jpeg")
            .shared_from_this();
    });
    router.get("/images/avatars/beagle.jpg", [](const Request& request) {
        return std::make_shared<Http::Response>()
            ->sendFile("../html/images/avatars/beagle.jpg")
            .mimetype("image/jpeg")
            .shared_from_this();
    });
    router.get("/images/avatars/panda.jpg", [](const Request& request) {
        return std::make_shared<Http::Response>()
            ->sendFile("../html/images/avatars/panda.jpg")
            .mimetype("image/jpeg")
            .shared_from_this();
    });
    router.get("/images/avatars/teddy.jpg", [](const Request& request) {
        return std::make_shared<Http::Response>()
            ->sendFile("../html/images/avatars/teddy.jpg")
            .mimetype("image/jpeg")
            .shared_from_this();
    });
    // T::defaultHandler(getDefaultHandler());
    // T::setPresentation(m_presentation);
    // T::finish_init();
    return *this;
}
LoginController::~LoginController() = default;

bool LoginController::isLoginAttempt(const map<string, string>& parameters)
{
    return parameters.count("username") && parameters.count("password");
}

bool LoginController::isValidUser(
    const map<string, string>& parameters, Data::User& user)
{
    return user.isValidUser(
        parameters.at("username"), parameters.at("password"), user);
}

shared_ptr<Response> LoginController::loginForm()
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
void LoginController::setPostProcessingHook(
    LoginController::PostProcessingHook hook)
{
    m_postProcessingHook = std::move(hook);
}

shared_ptr<Response> LoginController::forwardToSecretHandler(
    const Request& request)
{
    auto response
        = m_postProcessingHook(request, m_secretHandler->handle(request));
    Http::Session::addAlertToSession(request, *response);
    return response;
}
handler_type LoginController::getDefaultHandler()
{
    return [this](const Request& request) {
        if (m_adminHandler && Http::Session(request).isLoggedIn()
            && Http::Session(request).isAdmin()) {
            auto response = m_adminHandler->handle(request);
            if (response) {
                return m_postProcessingHook(request, response);
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
