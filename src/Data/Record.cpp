
#include "Record.hpp"

#include "Trace/trace.hpp"

#ifndef __clang__
#   pragma GCC diagnostic push
#   pragma GCC diagnostic ignored "-Wmaybe-uninitialized"
#endif
#include <regex>
#ifndef __clang__
#   pragma GCC diagnostic pop
#endif

KeyStringType::KeyStringType(const string& s)
    : string(s)
{
    // Note: Could enable this test only during test builds for performance
    // throw if invalid char
    // allowed: a-z, A-Z, 0-9, _
    if (s.find_first_not_of("abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRST"
                            "UVWXYZ0123456789_")
        != string::npos) {
        TRACE_THROW("Invalid character in KeyStringType: " + s);
    }
}
KeyStringType::KeyStringType(const char* s)
    : string(s)
{
    std::regex r("[a-zA-Z0-9_]+");
    if (!std::regex_match(s, r)) {
        TRACE_THROW("Invalid character in KeyStringType: " + string(s));
    }
}
