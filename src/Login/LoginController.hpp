#pragma once

#include "Data/User.hpp"
#include "Http/handler_type.hpp"

#include <set>
#include <map>
#include <string>
using std::set;
using std::string;
using std::map;

namespace Http {
class Response;
class RequestHandler;
class Router;
}
namespace Html {
struct Presentation;
}
struct LoginController {
    static bool isLoginAttempt(const map<string, string>& parameters);

    static bool isValidUser(
        const map<string, string>& parameters, Data::User& user);

    Http::handler_type getDefaultHandler();
    using RequestHandler=Http::RequestHandler;
    using Response=Http::Response;
    using Request=Http::Request;

    LoginController(
        shared_ptr<RequestHandler> secretHandler,
        shared_ptr<RequestHandler> adminHandler,
        shared_ptr<RequestHandler> publicHandler,
        shared_ptr<Html::Presentation> presentation,
        Http::Router& router);

    shared_ptr<Response> forwardToSecretHandler(const Request& request);

    [[nodiscard]] shared_ptr<Response> addLinksToResponse(
        const Request& request, shared_ptr<Response> response) const;

private:
    shared_ptr<RequestHandler> m_secretHandler;
    shared_ptr<RequestHandler> m_adminHandler;
    shared_ptr<RequestHandler> m_publicHandler;
    shared_ptr<Html::Presentation> m_presentation;

    shared_ptr<Response> loginForm();
};
