
#include "SessionStorage.hpp"

namespace Http {
std::map<SessionId, SessionData> SessionStorage::m_storage;

bool SessionStorage::pop(const SessionId& sessionId)
{
    if (m_storage.count(sessionId) > 0) {
        m_current = m_storage[sessionId];
        return true;
    }
    return false;
}
size_t SessionStorage::count(const SessionId& sessionId)
{
    return m_storage.count(sessionId);
}
SessionData& SessionStorage::data()
{
    return m_current;
}
void SessionStorage::erase(const SessionId& sessionId)
{
    m_storage.erase(sessionId);
}
void SessionStorage::insert(
    const SessionId& sessionId, const SessionData& sessionData)
{
    m_storage[sessionId] = sessionData;
}
void SessionStorage::clearAll()
{
    m_storage.clear();
}
struct SessionDataRecord : public Record {
    SessionDataRecord(SessionId id, const SessionData& data)
        : id(std::move(id))
        , data(data)
    {
    }
    string presentableName() const override
    {
        return "Session";
    }
    string key() const override
    {
        return id;
    };
    std::vector<KeyStringType> fields() const override
    {
        return {
            "id",
            "user_id",
            "is_logged_in",
            "createdAt",
            "lastUsedAt",
            "path",
            "userAgent"};
    };
    std::map<KeyStringType, string> values() const override
    {
        return {
            {"id", id},
            {"user_id", data.userId()},
            {"is_logged_in", data.isLoggedInConst() ? "true" : "false"},
            {"createdAt", data.createdAt()},
            {"lastUsedAt", data.lastUsedAt()},
            {"path", data.path()},
            {"userAgent", data.userAgent()},
        };
    };
    HtmlInputType inputType(const KeyStringType& field) const override
    {
        if (field == "id") {
            return HtmlInputType::TEXT;
        }
        if (field == "user_id") {
            return HtmlInputType::TEXT;
        }
        if (field == "is_logged_in") {
            return HtmlInputType::TEXT;
        }
        return HtmlInputType::TEXT;
    };
    SessionId id;
    const SessionData& data;
};

vector<shared_ptr<Record>> SessionStorage::listAll()
{
    vector<shared_ptr<Record>> result;
    result.reserve(m_storage.size());
    for (auto& [id, data] : m_storage) {
        result.push_back(make_shared<SessionDataRecord>(id, data));
    }
    return result;
}
} // namespace Http