#include "PocoPageHandler.hpp"

#include <utility>

using HTTPCookie = Poco::Net::HTTPCookie;

PocoPageHandler::PocoPageHandler(
    handler_type handler, shared_ptr<Presentation> presentation)
    : handler(std::move(handler))
    , m_presentation(std::move(presentation))
{
}
map<string, string> PocoPageHandler::convert(const NameValueCollection& cookies)
{
    map<string, string> cookiesMap;
    for (auto& [key, value] : cookies) {
        cookiesMap[key] = value;
    }
    return cookiesMap;
}
Http::Method PocoPageHandler::httpMethod(const string& requestMethod)
{
    auto method = Http::Method::GET;
    if (const map<string, Http::Method> methods
        = {{Poco::Net::HTTPRequest::HTTP_GET, Http::Method::GET},
           {Poco::Net::HTTPRequest::HTTP_POST, Http::Method::POST}};
        methods.find(requestMethod) == methods.end()) {
        method = Http::Method::METHOD_NOT_ALLOWED;
    } else {
        method = methods.at(requestMethod);
    }
    return method;
}
void PocoPageHandler::addCookiesToResponse(
    PocoPageHandler::HTTPServerResponse& response, const Response& result)
{
    for (auto& [key, value] : result.cookies()) {
        HTTPCookie cookie(key, value);
        cookie.setSameSite(HTTPCookie::SAME_SITE_STRICT);
        cookie.setPath("/");
        if (value.empty()) {
            cookie.setMaxAge(0);
        }
        response.addCookie(cookie);
    }
}
void PocoPageHandler::handleRequest(
    HTTPServerRequest& request, HTTPServerResponse& response)
{
    response.setChunkedTransferEncoding(true);
    response.setContentType("text/html");

    NameValueCollection cookies;
    request.getCookies(cookies);

    Poco::URI uri(request.getURI());

    spdlog::info("Request: {}", uri.toString());

    const Http::Method method = httpMethod(request.getMethod());
    if (method == Http::Method::METHOD_NOT_ALLOWED) {
        response.setStatus(Poco::Net::HTTPResponse::HTTP_METHOD_NOT_ALLOWED);
        response.send() << "Method Not Allowed\n";
        return;
    }

    const Request req(
        uri.getPath(),
        (convert(cookies)),
        (convert(HTMLForm(request, request.stream()))),
        uri.getQuery(),
        method,
        (request.get("User-Agent", "")));

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
    m_form = result->form();

    addCookiesToResponse(response, *result);

    response.setContentType(result->mimetype());
    if (result->status() == Response::HTTP_FOUND
        && (!result->location().empty())) {
        response.redirect(result->location());
        return;
    }
    if (result->sendFile()) {
        response.sendFile(result->filename(), result->mimetype());
    } else {
        auto& responseStream = response.send();
        responseStream << Html::Presentation::render(*result);
    }
}
Input::FormPtr PocoPageHandler::form() const
{
    return m_form;
}
