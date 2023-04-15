#include "doctest.h"
#include <Poco/Data/SQLite/Connector.h>
#include <Poco/Data/Session.h>
#include <Poco/Net/HTMLForm.h>
#include <Poco/Net/HTTPRequestHandler.h>
#include <Poco/Net/HTTPServer.h>
#include <Poco/Net/HTTPServerRequest.h>
#include <Poco/Net/HTTPServerResponse.h>
#include <Poco/Net/ServerSocket.h>
#include <Poco/URI.h>
#include <Poco/UUIDGenerator.h>
#include <Poco/Util/ServerApplication.h>
#include <functional>
#include <iostream>
#include <map>
#include <sstream>
#include <string>
#include <thread>
#include <vector>

#include "List.hpp"
#include "Table.hpp"
#include "todo.hpp"
#include <set>
#include <variant>

using namespace std;
using Poco::Data::Session;

namespace Input {
struct Element {
    virtual string operator()() = 0;
};


struct Text : public Element {
    Text(string label, string value = "") : m_label(move(label)), m_value(move(value)) {}

    string operator()() {
        ostringstream str;
        str << R"(<label for =")"
            << m_label << R"(">)" << m_label << R"(</label><br> <input type="text" id=")"
            << m_label << R"(" name=")"
            << m_label << R"(" value=")" << m_value << R"(">)";
        return str.str();
    }

private:
    string m_label;
    string m_value;
};

struct Password : public Element {
    Password(string label) : m_label(move(label)) {}

    string operator()() {
        ostringstream str;
        str << R"(<input type="password" id=")" << m_label << R"(" name=")" << m_label << R"(">)";
        return str.str();
    }

private:
    string m_label;
};

struct Submit : public Element {
    Submit(string label) : m_label(move(label)) {}

    string operator()() {
        ostringstream str;
        str << R"(<input type="submit" id=")" << m_label << R"(" name=")" << m_label << R"(">)";
        return str.str();
    }

private:
    string m_label;
};

struct Form : public Element {
    Form(vector<string> elements, string action, string method) : m_elements(move(elements)),
                                                                  m_action(move(action)),
                                                                  m_method(move(method)) {}

    Form(const Record &record, string action, string method) : m_action(move(action)),
                                                               m_method(move(method)) {
        for (const auto &[key, value]: record.values()) {
            m_elements.push_back(Text(key, value)());
        }
        m_elements.push_back(Submit("submit")());
    }

    string operator()() {
        ostringstream str;
        str << R"(<form action=")" << m_action << R"(" method=")" << m_method << R"(">)";
        for (auto element: m_elements) {
            str << element << "<br>\n";
        }
        str << "</form>";
        return str.str();
    }

private:
    vector<string> m_elements;
    string m_action;
    string m_method;
};

TEST_CASE("Text Element") {
    CHECK(Input::Text("username")() == R"(<input type="text" id="username" name="username">)");
}

TEST_CASE("Password Element") {
    CHECK(Input::Password("password")() == R"(<input type="password" id="password" name="password">)");
    CHECK(Input::Password("password2")() == R"(<input type="password" id="password2" name="password2">)");
}
}// namespace Input


struct Response : public enable_shared_from_this<Response> {
    static shared_ptr<Response> create() {
        return make_shared<Response>();
    }

    Response &content(const string &content) {
        m_content = content;
        return *this;
    }

    Response &cookie(const string &name, const string &value) {
        m_cookies[name] = value;
        return *this;
    }

    string content() const { return m_content; }

    map<string, string> cookies() const { return m_cookies; }

private:
    string m_content;
    map<string, string> m_cookies;
};

shared_ptr<Response> content(const string &content) {
    return Response::create()->content(content).shared_from_this();
}

TEST_CASE("Create Response") {
    auto response = content("Hello World");
}

struct Parameters {
    virtual ~Parameters() = default;
    virtual bool hasParameter(const string &name) const = 0;
    virtual string parameter(const string &name) const = 0;
};

struct Request {
    Request(map<string, string> cookies, map<string, string> parameters, string query) : m_cookies(move(cookies)),
                                                                                         m_parameters(move(parameters)),
                                                                                         m_query(move(query)) {}

    Request() = default;

    bool hasCookie(const string &name) const { return m_cookies.find(name) != m_cookies.end(); }

    string cookie(const string &name) const { return m_cookies.at(name); }

    bool hasParameter(const string &name) const { return m_parameters.find(name) != m_parameters.end(); }

    string parameter(const string &name) const { return m_parameters.at(name); }

    const map<string, string> &allParameters() const { return m_parameters; }

    string query() const { return m_query; }

private:
    map<string, string> m_cookies;
    map<string, string> m_parameters;
    string m_query;
};

// helper type for the visitor #4
template<class... Ts>
struct overloaded : Ts... {
    using Ts::operator()...;
};

// explicit deduction guide (not needed as of C++20)
template<class... Ts>
overloaded(Ts...) -> overloaded<Ts...>;

struct TestServer {
    using response_handler_type = function<shared_ptr<Response>()>;
    using request_response_handler_type = function<shared_ptr<Response>(const Request &request)>;
    using handlers_type = variant<response_handler_type, request_response_handler_type>;

    void get(const string &path, response_handler_type handler) {
        router[path] = handler;
    }

    void get(const string &path, request_response_handler_type handler) {
        router[path] = handler;
    }

    void post(const string &path, response_handler_type handler) {
        router[path] = handler;
    }

    void post(const string &path, request_response_handler_type handler) {
        router[path] = handler;
    }

    void finish_init() {}

    string
    getPage(const string &path, const map<string, string> &cookies = {}, const map<string, string> &params = {}) {
        if (router.find(path) != router.end()) {
            auto &handler = router[path];
            auto result = visit(overloaded{
                                        [](response_handler_type &handler) { return handler()->content(); },
                                        [&cookies, &params](request_response_handler_type &handler) {
                                            Request parameters(cookies, params, "");
                                            return handler(parameters)->content();
                                        }},
                                handler);
            return result;
        }
        return "";
    }

    shared_ptr<Response> postTo(const string &path, const map<string, string> &params) {
        if (router.find(path) != router.end()) {
            return visit(overloaded{
                                 [](response_handler_type &handler) { return handler(); },
                                 [params](request_response_handler_type &handler) {
                                     Request request({}, params, "");
                                     return handler(request);
                                 }},
                         router[path]);
        }
        return content("");
    }

private:
    map<string, handlers_type> router;
};

struct SessionId : public string {
    SessionId() = default;

    explicit SessionId(string uuid) : uuid(uuid) {
        assign(uuid);
    }

    explicit SessionId(Poco::UUID uuid) : uuid(uuid) {
        assign(uuid.toString());
    }

    SessionId &operator=(const Poco::UUID &b) {
        uuid = b;
        assign(uuid.toString());
        return *this;
    }

    Poco::UUID asUUID() const {
        return uuid;
    }

private:
    Poco::UUID uuid;
};

TEST_CASE("SessionId") {
    SUBCASE("set") {
        SessionId sessionOne(Poco::UUIDGenerator::defaultGenerator().createRandom());
        SessionId sessionTwo(Poco::UUIDGenerator::defaultGenerator().createRandom());
        CHECK(sessionOne != sessionTwo);
        SessionId sessionThree = sessionTwo;
        CHECK(sessionThree == sessionTwo);
        set<SessionId> sessions;
        CHECK(sessions.count(sessionOne) == 0);
        CHECK(sessions.count(sessionTwo) == 0);
        sessions.insert(sessionOne);
        CHECK(sessions.count(sessionOne) == 1);
        CHECK(sessions.count(sessionTwo) == 0);
        sessions.erase(sessionOne);
        CHECK(sessions.count(sessionOne) == 0);
        CHECK(sessions.count(sessionTwo) == 0);
    }
    SUBCASE("convert") {
        SessionId sessionOne(Poco::UUIDGenerator::defaultGenerator().createRandom());
        string sessionIdAsString = sessionOne;
        SessionId sessionIdFromString{sessionIdAsString};
        CHECK(sessionOne == sessionIdFromString);
        CHECK(sessionOne.asUUID() == sessionIdFromString.asUUID());
    }
}

SessionId generateRandomSessionId() {
    return SessionId{Poco::UUIDGenerator::defaultGenerator().createRandom()};
}

template<typename T>
struct LoginServer : public T {
    static bool isLoginAttempt(const map<string, string> &parameters) {
        return parameters.count("username") && parameters.count("password");
    }

    static bool isValidUser(const map<string, string> &parameters) {
        const auto username = "admin";
        const auto password = "Adm1n!";
        return (parameters.at("username") == username) && (parameters.at("password") == password);
    }

    bool hasValidSession(const Request &request) const {
        return request.hasCookie("session-id") &&
               m_sessions.count(SessionId{request.cookie("session-id")});
    }

    void clearSession(const Request &request) {
        if (request.hasCookie("session-id")) {
            m_sessions.erase(SessionId{request.cookie("session-id")});
        }
    }

    LoginServer() {
        T::get("/", [] {
            using namespace Input;
            auto text = Form({Text("username")(),
                              Password("password")(),
                              Submit("submit")()},
                             "/login",
                             "post")();
            auto result = content(text);
            return result;
        });
        T::post("/login", [this](const Request &request) {
            if (!isLoginAttempt(request.allParameters())) {
                return content("Invalid Request");
            }
            if (!isValidUser(request.allParameters())) {
                return content("Invalid Login");
            }
            auto sessionId = generateRandomSessionId();
            m_sessions.insert(sessionId);
            return content("Success")->cookie("session-id", sessionId).shared_from_this();
        });
        T::get("/secret", [this](const Request &request) {
            if (hasValidSession(request)) {
                return content("Success");
            } else {
                return content("Access denied");
            }
        });
        T::get("/logout", [this](const Request &request) {
            if (hasValidSession(request)) {
                clearSession(request);
                return content("Logged out")->cookie("session-id", "").shared_from_this();
            } else {
                return content("Access denied");
            }
        });
        T::finish_init();
    }

private:
    set<SessionId> m_sessions;
};

/*
 * A Simple Todo List:
 * Data Model:
 *
 * id (uuid)
 * Description (text)
 * Checked (bool)
 * created_at (date)
 * updated_at (date)
 *
 * CREATE TABLE
 * INSERT
 * UPDATE
 * DELETE FROM
 *
 * ActiveRecord {
 *   uuid id;
 *   string description;
 *   bool checked;
 *   std::chrono::time_point<std::chrono::system_clock> created_at;
 *   std::chrono::time_point<std::chrono::system_clock> updated_at;
 *   update();
 *   insert();
 *   delete();
 * }
 *
 * https://stackoverflow.com/questions/23038996/stdchronohigh-resolution-clocktime-point-cast-to-store-in-sqlite
 *     using namespace std::literals; // enables the usage of 24h, 1ms, 1s instead of
                                   // e.g. std::chrono::hours(24), accordingly
    const std::chrono::time_point<std::chrono::system_clock> now =
        std::chrono::system_clock::now();
 */

template<typename T>
struct CrudServer : public T {
    CrudServer() {
        T::get("/new", [](const Request &request) {
            using namespace Input;
            Todo todo;
            return content(R"(<a href="/list">list</a><br>)" +
                           Form(todo, "/create", "post")());
        });
        T::post("/create", [this](const Request &request) {
            Todo todo;
            for (auto i: todo.fields()) {
                if (request.hasParameter(i)) {
                    todo.set(i, request.parameter(i));
                }
            }
            todo.insert();
            return content(string{"Todo created<br>"} + R"(<a href="/list">list</a><br>)" +
                           Input::Form(todo, "/update", "post")());
            //return redirect_to("/read/" + id.toString());
        });
        T::get("/edit", [](const Request &request) {
            Todo todo;
            todo.pop(request.query());
            return content(R"(<a href="/list">list</a><br>)" +
                           Input::Form(todo, "/update", "post")());
        });
        T::post("/update", [](const Request &request) {
            Todo todo;
            todo.pop(request.query());
            for (auto i: todo.fields()) {
                if (request.hasParameter(i)) {
                    todo.set(i, request.parameter(i));
                }
            }
            todo.update();
            return content(string{"Todo updated<br>"} + R"(<a href="/list">list</a><br>)" +
                           Input::Form(todo, "/edit-submit", "post")());
        });
        T::get("/delete", [](const Request &request) {
            return content("");
        });
        T::get("/list", [](const Request &request) {
            return content(R"(<a href="/new">Create new todo</a><br>)" +
                           Html::List(Todo::listAsPointers())());
        });
        T::finish_init();
    }
};

/*
 * Test Cases:
 *
 * Redirect to edit after create
 * Redirect to edit after update
 * Flash Banner after Redirect
 * Confirm before delete (Javascript?)
 */

TEST_CASE("Crud Server") {
    CrudServer<TestServer> w;
    SUBCASE("UUID to string") {
        Poco::UUID id("7f74fe03-d834-4d5e-bde7-58712c755781");
        CHECK(id.toString() == "7f74fe03-d834-4d5e-bde7-58712c755781");
    }
    SUBCASE("Poco URI") {
        Poco::URI uri1("/read?7f74fe03-d834-4d5e-bde7-58712c755781");
        CHECK_EQ(uri1.getPath(), "/read");
        CHECK_EQ(uri1.getQuery(), "7f74fe03-d834-4d5e-bde7-58712c755781");
    }
}

TEST_CASE("Login Server") {
    LoginServer<TestServer> w;
    auto actual = w.getPage("/");
    const auto expected = R"(<form action="/login" method="post"><input type="text" id="username" name="username"><input type="password" id="password" name="password"><input type="submit" id="submit" name="submit"></form>)";
    CHECK(actual == expected);
    SUBCASE("Login") {
        CHECK(w.getPage("/secret") == "Access denied");
        map<string, string> params;
        params["username"] = "admin";
        params["password"] = "Adm1n!";
        auto response = w.postTo("/login", params);
        CHECK(w.getPage("/secret", response->cookies()) == "Success");
    }
}

class PocoWebServer {
public:
    using response_handler_type = function<shared_ptr<Response>()>;
    using request_response_handler_type = function<shared_ptr<Response>(const Request &request)>;
    using handlers_type = variant<response_handler_type, request_response_handler_type>;

    using HTTPServer = Poco::Net::HTTPServer;

    void get(const string &path, request_response_handler_type handler) {
        router[path] = handler;
    }

    void get(const string &path, response_handler_type handler) {
        router[path] = handler;
    }

    void post(const string &path, response_handler_type handler) {
        router[path] = handler;
    }

    void post(const string &path, request_response_handler_type handler) {
        router[path] = handler;
    }

    void finish_init() {
        using ServerSocket = Poco::Net::ServerSocket;
        using HTTPServerParams = Poco::Net::HTTPServerParams;
        using HTTPRequestHandlerFactory = Poco::Net::HTTPRequestHandlerFactory;
        using HTTPRequestHandler = Poco::Net::HTTPRequestHandler;
        using HTTPServerRequest = Poco::Net::HTTPServerRequest;
        using HTTPServerResponse = Poco::Net::HTTPServerResponse;
        using HTMLForm = Poco::Net::HTMLForm;
        using NameValueCollection = Poco::Net::NameValueCollection;
        ServerSocket socket(8080);
        auto pParams = new HTTPServerParams();
        class HandlerFactory : public HTTPRequestHandlerFactory {
        public:
            HandlerFactory(map<string, handlers_type> &router) : router(router) {}

            HTTPRequestHandler *createRequestHandler(const HTTPServerRequest &request) {
                class PageHandler : public HTTPRequestHandler {
                public:
                    PageHandler(handlers_type handler) : handler(handler) {}

                    void handleRequest(HTTPServerRequest &request, HTTPServerResponse &response) override {
                        response.setChunkedTransferEncoding(true);
                        response.setContentType("text/html");
                        using HTTPCookie = Poco::Net::HTTPCookie;
                        /*HTTPCookie cookie("session-id",
                                          Poco::UUIDGenerator::defaultGenerator().createRandom().toString());
                        response.addCookie(cookie);*/
                        NameValueCollection cookies;
                        request.getCookies(cookies);
                        Poco::URI uri(request.getURI());
                        HTMLForm form(request, request.stream());
                        visit(overloaded{
                                      [&response](response_handler_type &handler) {
                                          auto result = handler();
                                          for (auto &[key, value]: result->cookies()) {
                                              response.addCookie(HTTPCookie(key, value));
                                          }
                                          auto &responseStream = response.send();
                                          responseStream << result->content();
                                      },
                                      [&form, &response, &cookies, &uri](request_response_handler_type &handler) {
                                          map<string, string> parameters;
                                          map<string, string> cookiesMap;
                                          for (auto &[key, value]: cookies) {
                                              cookiesMap[key] = value;
                                          }
                                          for (auto &[key, value]: form) {
                                              parameters[key] = value;
                                          }
                                          auto result = handler(Request(cookiesMap, parameters, uri.getQuery()));
                                          for (auto &[key, value]: result->cookies()) {
                                              HTTPCookie cookie(key, value);
                                              if (value.empty()) {
                                                  cookie.setMaxAge(0);
                                              }
                                              response.addCookie(cookie);
                                          }
                                          auto &responseStream = response.send();
                                          responseStream << result->content();
                                      }},
                              handler);
                    }

                private:
                    handlers_type handler;
                };
                struct EmptyHandler : public HTTPRequestHandler {
                    void handleRequest(HTTPServerRequest &request, HTTPServerResponse &response) override {
                        response.setChunkedTransferEncoding(true);
                        response.setContentType("text/html");
                        auto &responseStream = response.send();
                        responseStream << "Not found!";
                    }
                };
                auto uri = Poco::URI(request.getURI());
                if (router.find(uri.getPath()) != router.end()) {
                    return new PageHandler(router[uri.getPath()]);
                }
                return new EmptyHandler();
            }

        private:
            map<string, handlers_type> &router;
        };
        server = make_shared<HTTPServer>(new HandlerFactory(router), socket, pParams);
    }

    void start() { server->start(); }

    void stop() { server->stop(); }

private:
    map<string, handlers_type> router;
    shared_ptr<HTTPServer> server;
};

TEST_CASE("Web Server") {
    TestServer w;
    w.get("/", [] { return content("Hello World"); });
}


class ServerApplication : public Poco::Util::ServerApplication {
protected:
    int main(const vector<string> &args) {
        LoginServer<PocoWebServer> server;
        server.start();
        waitForTerminationRequest();
        server.stop();
        return EXIT_OK;
    }
};

class CrudApplication : public Poco::Util::ServerApplication {
protected:
    int main(const vector<string> &args) {
        CrudServer<PocoWebServer> server;
        server.start();
        waitForTerminationRequest();
        server.stop();
        return EXIT_OK;
    }
};

TEST_CASE("Poco Web Server") {
    try {
        Poco::Data::SQLite::Connector::registerConnector();
        Session session("SQLite", ":memory:");
        g_session = &session;
        Todo::create_table();
        Todo todo =
                {
                        "0123",
                        "Buy Milk",
                        "",
                        "",
                        0};
        todo.insert();
        todo =
                {
                        "0123",
                        "Empty Trash",
                        "",
                        "",
                        0};
        todo.insert();
        CrudApplication app;
        static char buffer[MAX_INPUT];
        strncpy(buffer, "app", MAX_INPUT);
        char *argv[] = {buffer};
        app.run(1, argv);
    } catch (Poco::Exception &exc) {
        cerr << exc.displayText() << endl;
    }
}