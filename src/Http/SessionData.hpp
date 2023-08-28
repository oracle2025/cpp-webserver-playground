#pragma once

#include "Data/User.hpp"
#include "Html/Alert.hpp"

#include <optional>

namespace Http {
using std::optional;
using std::string;

class SessionData {
public:
    using Alert = Html::Alert;

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

private:
    optional<Alert> m_alert;
    bool m_isLoggedIn = false;
    string m_userId;
    string m_userName;
    std::time_t m_createdAt = now();
    std::time_t m_lastUsedAt = now();
    static std::time_t now();
    void touch();
};

} // namespace Http
