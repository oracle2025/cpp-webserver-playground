#pragma once

#include "Login/SessionId.hpp"

#include <set>

namespace Http {

class Request;
class Response;
using std::set;

class Session {
public:
    Session(const Request& request);
    bool hasValidSession() const;
    void clearSession();
    void createSession(Response& response);

private:
    const Request& request;
    static set<SessionId> m_sessions;
};

} // namespace Http
