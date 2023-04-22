#pragma once

#include "Form.hpp"
#include "Password.hpp"
#include "Request.hpp"
#include "Response.hpp"
#include "SessionId.hpp"
#include "Submit.hpp"
#include "Text.hpp"

#include <map>
#include <set>
#include <string>
using std::map;
using std::set;
using std::string;

template<typename T>
struct LoginServer : public T {
    static bool isLoginAttempt(const map<string, string>& parameters)
    {
        return parameters.count("username") && parameters.count("password");
    }

    static bool isValidUser(const map<string, string>& parameters)
    {
        const auto username = "admin";
        const auto password = "Adm1n!";
        return (parameters.at("username") == username)
            && (parameters.at("password") == password);
    }

    bool hasValidSession(const Request& request) const
    {
        return request.hasCookie("session-id")
            && m_sessions.count(SessionId{request.cookie("session-id")});
    }

    void clearSession(const Request& request)
    {
        if (request.hasCookie("session-id")) {
            m_sessions.erase(SessionId{request.cookie("session-id")});
        }
    }

    LoginServer()
    {
        T::get("/", [] {
            using namespace Input;
            auto text = Form(
                {Text("username")(),
                 Password("password")(),
                 Submit("submit")()},
                "/login",
                "post")();
            const string header = R"(<!doctype html><html lang="de"><head>
<meta charset="utf-8">
<meta name="viewport" content="width=device-width, initial-scale=1.0">
<link rel="stylesheet" type="text/css" href="css/style.css">
</head>
<body>
)";
            const string footer = R"(</body></html>)";
            auto result = content(header + text + footer);
            return result;
        });
        T::post("/login", [this](const Request& request) {
            if (!isLoginAttempt(request.allParameters())) {
                return content("Invalid Request");
            }
            if (!isValidUser(request.allParameters())) {
                return content("Invalid Login");
            }
            auto sessionId = generateRandomSessionId();
            m_sessions.insert(sessionId);
            return content("Success")
                ->cookie("session-id", sessionId)
                .shared_from_this();
        });
        T::get("/secret", [this](const Request& request) {
            if (hasValidSession(request)) {
                return content("Success");
            } else {
                return content("Access denied");
            }
        });
        T::get("/logout", [this](const Request& request) {
            if (hasValidSession(request)) {
                clearSession(request);
                return content("Logged out")
                    ->cookie("session-id", "")
                    .shared_from_this();
            } else {
                return content("Access denied");
            }
        });
        T::get("/css/style.css", [] {
            return content(R"(body {
font-family: Arial, sans-serif;
background-color: #f4f4f4;
}
.container {
background-color: #fff;
padding: 20px;
max-width: 500px;
margin: auto;
margin-top: 50px;
box-shadow: 0px 0px 10px rgba(0,0,0,0.2);
}
h2 {
text-align: center;
margin-bottom: 20px;
}
input[type=text], input[type=password] {
width: 100%;
padding: 12px 20px;
margin: 8px 0;
display: inline-block;
border: 1px solid #ccc;
border-radius: 4px;
box-sizing: border-box;
}
button[type=submit] {
background-color: #4CAF50;
color: white;
padding: 14px 20px;
margin: 8px 0;
border: none;
border-radius: 4px;
cursor: pointer;
width: 100%;
}
button[type=submit]:hover {
background-color: #45a049;
})", "text/css");});
        T::finish_init();
    }

private:
    set<SessionId> m_sessions;
};
