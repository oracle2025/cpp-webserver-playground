#pragma once

#include "Html/Alert.hpp"

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

private:
    optional<Alert> m_alert;
};

} // namespace Http
