#pragma once
#include "SessionId.hpp"
#include "Http/SessionData.hpp"
namespace Http {

class SessionStorageSql {
public:
    bool pop(const SessionId& sessionId);
        static size_t count(const SessionId& sessionId);
        SessionData& data();
        static void erase(const SessionId& sessionId);
        static void insert(const SessionId& sessionId, const SessionData& sessionData);
        static void clearAll();
        static vector<shared_ptr<Record>> listAll();
    private:
        SessionData m_current;
};

} // namespace Http

