#pragma once

#include "Request.hpp"
#include "SessionId.hpp"
#include "Text.hpp"
#include "Password.hpp"
#include "Submit.hpp"
#include "Form.hpp"
#include "Response.hpp"
#include <string>
#include <map>
#include <set>
using std::string;
using std::map;
using std::set;

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
        return request.hasCookie("session-m_id") &&
            m_sessions.count(SessionId{request.cookie("session-m_id")});
    }

    void clearSession(const Request &request) {
        if (request.hasCookie("session-m_id")) {
            m_sessions.erase(SessionId{request.cookie("session-m_id")});
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
            return content("Success")->cookie("session-m_id", sessionId).shared_from_this();
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
                return content("Logged out")->cookie("session-m_id", "").shared_from_this();
            } else {
                return content("Access denied");
            }
        });
        T::finish_init();
    }

private:
    set<SessionId> m_sessions;
};
