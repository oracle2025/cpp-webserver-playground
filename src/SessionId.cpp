#include "SessionId.hpp"

#include "doctest.h"
#include <set>
#include <Poco/UUIDGenerator.h>

using std::set;

TEST_CASE("SessionId") {
    SUBCASE("set") {
        SessionId sessionOne(Poco::UUIDGenerator::defaultGenerator().createRandom());
        SessionId sessionTwo(Poco::UUIDGenerator::defaultGenerator().createRandom());
        CHECK(sessionOne != sessionTwo);
        SessionId sessionThree = sessionTwo;
        CHECK(sessionThree == sessionTwo);
        set<SessionId> sessions;
        CHECK(sessions.count(sessionOne) == 0);
        CHECK(sessions.count(sessionTwo) == 0);
        sessions.insert(sessionOne);
        CHECK(sessions.count(sessionOne) == 1);
        CHECK(sessions.count(sessionTwo) == 0);
        sessions.erase(sessionOne);
        CHECK(sessions.count(sessionOne) == 0);
        CHECK(sessions.count(sessionTwo) == 0);
    }
    SUBCASE("convert") {
        SessionId sessionOne(Poco::UUIDGenerator::defaultGenerator().createRandom());
        string sessionIdAsString = sessionOne;
        SessionId sessionIdFromString{sessionIdAsString};
        CHECK(sessionOne == sessionIdFromString);
        CHECK(sessionOne.asUUID() == sessionIdFromString.asUUID());
    }
}
SessionId::SessionId(string uuid)
    : uuid(uuid) {
    assign(uuid);
}
SessionId::SessionId(Poco::UUID uuid)
    : uuid(uuid) {
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
