#pragma once

#include "Html/Alert.hpp"
#include "Data/User.hpp"

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

    bool isLoggedIn() const;

    string userId() const;
    string userName() const;

private:
    optional<Alert> m_alert;
    bool m_isLoggedIn = false;
    string m_userId;
    string m_userName;
};

} // namespace Http
