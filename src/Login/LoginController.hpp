#pragma once

#include "Data/User.hpp"
#include "Http/handler_type.hpp"

#include <map>
#include <set>
#include <string>
using std::map;
using std::set;
using std::string;

namespace Http {
class Response;
class RequestHandler;
class Router;
} // namespace Http
namespace Html {
struct Presentation;
}
struct LoginController : public std::enable_shared_from_this<LoginController> {
    static bool isLoginAttempt(const map<string, string>& parameters);

    static bool isValidUser(
        const map<string, string>& parameters, Data::User& user);

    Http::handler_type getDefaultHandler();
    using RequestHandler = Http::RequestHandler;
    using Response = Http::Response;
    using Request = Http::Request;

    LoginController(
        shared_ptr<RequestHandler> secretHandler,
        shared_ptr<RequestHandler> adminHandler,
        shared_ptr<RequestHandler> publicHandler,
        shared_ptr<Html::Presentation> presentation);
    LoginController& initialize(Http::Router& router);
    virtual ~LoginController();

    shared_ptr<Response> forwardToSecretHandler(const Request& request);


    using PostProcessingHook = std::function<shared_ptr<Response>(
        const Request& request, shared_ptr<Response> response)>;

    void setPostProcessingHook(PostProcessingHook hook);

private:
    shared_ptr<RequestHandler> m_secretHandler;
    shared_ptr<RequestHandler> m_adminHandler;
    shared_ptr<RequestHandler> m_publicHandler;
    shared_ptr<Html::Presentation> m_presentation;
    PostProcessingHook m_postProcessingHook;

    shared_ptr<Response> loginForm();
};
