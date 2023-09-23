#include "PocoPageHandler.hpp"
PocoPageHandler::PocoPageHandler(
    handler_type handler, shared_ptr<Presentation> presentation)
    : handler(handler)
    , m_presentation(presentation)
{
}
void PocoPageHandler::handleRequest(
    PocoPageHandler::HTTPServerRequest& request,
    PocoPageHandler::HTTPServerResponse& response)
{
    response.setChunkedTransferEncoding(true);
    response.setContentType("text/html");
    using HTTPCookie = Poco::Net::HTTPCookie;
    NameValueCollection cookies;
    request.getCookies(cookies);
    Poco::URI uri(request.getURI());
    spdlog::info("Request: {}", uri.toString());
    HTMLForm form(request, request.stream());
    map<string, string> parameters;
    map<string, string> cookiesMap;
    for (auto& [key, value] : cookies) {
        cookiesMap[key] = value;
    }
    for (auto& [key, value] : form) {
        parameters[key] = value;
    }
    const map<string, Http::Method> methods
        = {{Poco::Net::HTTPRequest::HTTP_GET, Http::Method::GET},
           {Poco::Net::HTTPRequest::HTTP_POST, Http::Method::POST}};
    if (methods.find(request.getMethod()) == methods.end()) {
        response.setStatus(Poco::Net::HTTPResponse::HTTP_METHOD_NOT_ALLOWED);
        response.send() << "Method Not Allowed\n";
        return;
    }
    const Http::Method method = methods.at(request.getMethod());
    string userAgent{""};
    if (request.has("User-Agent")) {
        userAgent = request.get("User-Agent");
    }
    const Request req(
        uri.getPath(), cookiesMap, parameters, uri.getQuery(), method, userAgent);
    shared_ptr<Response> result;
    try {
        result = handler(req);
    } catch (...) {
        std::ostringstream str;
        Trace::backtrace(std::current_exception(), str);
        spdlog::error("Exception: {}", str.str());
        using HTTPResponse = Poco::Net::HTTPResponse;
        response.setStatus(HTTPResponse::HTTP_INTERNAL_SERVER_ERROR);
        response.send() << "Internal Server Error\n";
        return;
    }
    Http::Session::addAlertToSession(req, *result);
    for (auto& [key, value] : result->cookies()) {
        HTTPCookie cookie(key, value);
        cookie.setSameSite(HTTPCookie::SAME_SITE_STRICT);
        cookie.setPath("/");
        if (value.empty()) {
            cookie.setMaxAge(0);
        }
        response.addCookie(cookie);
    }
    response.setContentType(result->mimetype());
    if (result->status() == Response::HTTP_FOUND
        && (!result->location().empty())) {
        response.redirect(result->location());
    }
    if (result->sendFile()) {
        response.sendFile(result->filename(), result->mimetype());
    } else {
        auto& responseStream = response.send();
        responseStream << m_presentation->render(*result);
    }
}
