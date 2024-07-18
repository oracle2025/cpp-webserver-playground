#pragma once
#include <stdexcept>


namespace Data {

class ValidationError : public std::runtime_error {
public:
        explicit ValidationError(const std::string& message);
};

} // namespace Data

