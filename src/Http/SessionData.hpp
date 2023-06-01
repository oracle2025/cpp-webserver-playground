#pragma once

#include "Html/Alert.hpp"
#include "Data/User.hpp"

#include <optional>

namespace Http {
using std::optional;

class SessionData {
public:
    using Alert = Html::Alert;

    void alert(const Alert& alert);

    void clearAlert();

    bool hasAlert() const;

    const Alert& getAlert() const;

    void login(User& user);

    void logout();

    bool isLoggedIn() const;

private:
    optional<Alert> m_alert;
    bool m_isLoggedIn = false;
};

} // namespace Http
