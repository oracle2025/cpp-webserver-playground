#pragma once

#include "Data/User.hpp"
#include "Html/Alert.hpp"
#include "SessionId.hpp"

#include <optional>
namespace Data {
struct SessionRowDefinition;
} // namespace Data

namespace Http {
using std::optional;
using std::string;

class SessionData {
public:
    using Alert = Html::Alert;

    SessionData(SessionId id, const string path, const string& userAgent);
    SessionData() = default;

    void alert(const Alert& alert);

    void clearAlert();

    bool hasAlert() const;

    const Alert& getAlert() const;

    void login(Data::User& user);

    void logout();

    bool isLoggedIn();

    bool isLoggedInConst() const;

    string userId() const;
    string userName() const;

    string createdAt() const;
    string lastUsedAt() const;

    string path() const;
    string userAgent() const;

private:
    optional<Alert> m_alert;
    bool m_isLoggedIn = false;
    string m_userId;
    string m_userName;
    std::time_t m_createdAt = now();
    std::time_t m_lastUsedAt = now();
    static std::time_t now();
    void touch();
    string m_path;
    string m_userAgent;
    SessionId m_id;
    friend struct Data::SessionRowDefinition;
};

} // namespace Http
