#pragma once

#include <stdexcept>
#include <string>
#include "backward.hpp"

namespace Trace {

class Exception : public std::runtime_error {
    std::string _message;
    std::string filename;
    unsigned int line;
    backward::StackTrace stacktrace;
public:
    Exception(
        const std::string& message, const char* filename, unsigned int line);

    ~Exception() throw() = default;

    const char * what() const throw()
    {
        return _message.c_str();
    }
};

} // namespace Trace
