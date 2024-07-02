
#pragma once

#include "Http/SessionData.hpp"
#include "SessionId.hpp"

#include <map>

namespace Http {

class Request;
class Response;
using std::map;

class Session {
public:
    Session(const Request& request);
    bool isLoggedIn() const;
    bool isAdmin() const;
    string userId() const;
    string userName() const;
    void clearSession();
    SessionData createSession(Response& response);
    SessionData current(Response& response);
    bool hasSession(Response& response) const;
    static void addAlertToSession(const Request& request, Response& response);

    static vector<shared_ptr<Record>> listAll();
    static void clearAll();

private:
    const Request& request;
};

} // namespace Http
