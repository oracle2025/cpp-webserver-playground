#pragma once
#include <string>
#include <Poco/UUID.h>
using std::string;

struct SessionId : public string {
    SessionId() = default;

    explicit SessionId(string uuid);

    explicit SessionId(Poco::UUID uuid);

    SessionId &operator=(const Poco::UUID &b);

    Poco::UUID asUUID() const;

private:
    Poco::UUID uuid;
};


SessionId generateRandomSessionId();