
#include "ValidationError.hpp"

namespace Data {
ValidationError::ValidationError(const std::string& message)
    : std::runtime_error(message)
{
}
} // namespace Data