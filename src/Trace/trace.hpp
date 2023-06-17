#pragma once

#include "Exception.hpp"
namespace Trace {

void backtrace(std::exception_ptr ex, std::ostream& out = std::cerr, int level = 0);
} // namespace Trace

#define TRACE_THROW(message)                                                   \
    throw Trace::Exception(message, __FILE__, __LINE__);

#define TRACE_RETHROW(message)                                                 \
    std::throw_with_nested(Trace::Exception(message, __BASE_FILE__, __LINE__));

#define TRACE_HANDLE(exception)