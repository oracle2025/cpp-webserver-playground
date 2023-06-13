#include "SessionId.hpp"

#include <Poco/UUIDGenerator.h>

#include <set>

using std::set;

SessionId::SessionId(string uuid)
    : uuid(uuid)
{
    assign(uuid);
}
SessionId::SessionId(Poco::UUID uuid)
    : uuid(uuid)
{
    assign(uuid.toString());
}
SessionId& SessionId::operator=(const Poco::UUID& b)
{
    uuid = b;
    assign(uuid.toString());
    return *this;
}
Poco::UUID SessionId::asUUID() const
{
    return uuid;
}

SessionId generateRandomSessionId()
{
    return SessionId{Poco::UUIDGenerator::defaultGenerator().createRandom()};
}
