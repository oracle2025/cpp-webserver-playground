#pragma once

#include "PocoPageHandler.hpp"

#include <string>
#include <regex>

struct FakeHTTPServerParams : public Poco::Net::HTTPServerParams {
    ~FakeHTTPServerParams() override = default;
};
struct FakeHTTPServerRequest : public PocoPageHandler::HTTPServerRequest {

    FakeHTTPServerRequest(Poco::Net::HTTPServerResponse& response)
        : m_response(response)
    {
    }
    std::istream& stream() override
    {
        return m_stream;
    }

    const Poco::Net::SocketAddress& clientAddress() const override
    {
        return m_clientAddress;
    }

    const Poco::Net::SocketAddress& serverAddress() const override
    {
        return m_serverAddress;
    }

    const Poco::Net::HTTPServerParams& serverParams() const override
    {
        return m_params;
    }

    Poco::Net::HTTPServerResponse& response() const override
    {
        return m_response;
    }

    bool secure() const override
    {
        return false;
    }

    void setStreamValue(const string& value)
    {
        m_stream.str(value);
    }

private:
    std::istringstream m_stream;
    Poco::Net::SocketAddress m_clientAddress;
    Poco::Net::SocketAddress m_serverAddress;
    Poco::Net::HTTPServerResponse& m_response;
    FakeHTTPServerParams m_params;
};

struct FakeHTTPServerResponse : public PocoPageHandler::HTTPServerResponse {
    void sendContinue()
    {
    }
    std::ostream& send()
    {
        return m_stream;
    }
    void sendFile(const std::string& path, const std::string& mediaType)
    {
    }
    void sendBuffer(const void* pBuffer, std::size_t length)
    {
    }
    void redirect(const std::string& uri, HTTPStatus status = HTTP_FOUND)
    {
        setStatus(status);
        set("Location", uri);
    }
    void requireAuthentication(const std::string& realm)
    {
    }
    bool sent() const
    {
        return false;
    }
    string result() const
    {
        return m_stream.str();
    }
    void reset()
    {
        setStatus(HTTP_OK);
        clear();
        m_stream.str("");
    }

private:
    std::ostringstream m_stream;
};

struct FakeBrowser {
    using string = std::string;
    FakeBrowser(PocoPageHandler& handler)
        : m_handler(handler)
    {
    }
    void location(
        const string& l,
        const string& method = Poco::Net::HTTPRequest::HTTP_GET,
        const string& streamValue = "")
    {
        FakeHTTPServerResponse response;
        FakeHTTPServerRequest request(response);
        request.setURI(l);
        request.setMethod(method);
        request.setCookies(m_cookieJar);
        Poco::URI uri(l);
        m_scheme = uri.getScheme();
        m_host = uri.getHost();
        m_location = l;
        request.setStreamValue(streamValue);
        m_handler.handleRequest(request, response);
        std::vector<Poco::Net::HTTPCookie> cookies;
        response.getCookies(cookies);
        for (auto& cookie : cookies) {
            m_cookieJar.set(cookie.getName(), cookie.getValue());
        }
        while (response.getStatus()
               == Poco::Net::HTTPResponse::HTTPStatus::HTTP_FOUND) {
            m_location = uri.getScheme() + "://" + uri.getHost()
                + response["Location"];
            response.reset();
            request.setURI(m_location);
            request.setCookies(m_cookieJar);
            // look at response headers
            request.setMethod(Poco::Net::HTTPRequest::HTTP_GET);
            request.setStreamValue("");
            m_handler.handleRequest(request, response);
        }
        m_form = m_handler.form();
        m_pageContents = response.result();
    }
    string location() const
    {
        return m_location;
    }
    static std::vector<std::tuple<std::string, std::string>> extractLinks(
        const std::string& html)
    {
        std::vector<std::tuple<std::string, std::string>> links;

        const std::regex linkRegex(
            "<a\\s+(?:[^>]*?\\s+)?href=\"([^\"]*)\".*?>(.*?)<\\/a>",
            std::regex::icase);
        std::smatch match;

        std::string::const_iterator searchStart(html.cbegin());
        while (std::regex_search(searchStart, html.cend(), match, linkRegex)) {
            std::string url = match[1].str();
            std::string linkText = match[2].str();

            links.emplace_back(linkText, url);

            searchStart = match.suffix().first;
        }

        return links;
    }
    bool follow_link(const string& link_text)
    {
        auto links = extractLinks(m_pageContents);
        for (auto& link : links) {
            if (std::get<0>(link) == link_text) {
                Poco::URI uri(std::get<1>(link));
                location(std::get<1>(link));
                return true;
            }
        }
        return false;
    }
    bool find_and_follow(const string& needle, int offset)
    {
        auto links = extractLinks(m_pageContents);
        std::vector<std::string> found;
        for (auto& link : links) {
            auto link_text = std::get<0>(link);
            if (link_text.find(needle) != std::string::npos) {
                found.push_back(std::get<1>(link));
            }
        }
        if (offset < found.size()) {
            location(found.at(offset));
            return true;
        }
        return false;
    }
    Input::FormPtr form()
    {
        return m_form;
    }
    string pageContents()
    {
        return m_pageContents;
    }
    void submit()
    {
        auto l = m_scheme + "://" + m_host + form()->action();
        location(l, Poco::Net::HTTPRequest::HTTP_POST, form()->data());
    }

private:
    PocoPageHandler& m_handler;
    Poco::Net::NameValueCollection m_cookieJar;
    string m_pageContents;
    Input::FormPtr m_form;
    string m_scheme;
    string m_host;
    string m_location;
};
