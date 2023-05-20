
#include "createRandomUUID.hpp"

#include <Poco/UUIDGenerator.h>

namespace String {

string createRandomUUID()
{
    return Poco::UUIDGenerator::defaultGenerator().createRandom().toString();
}
} // namespace String