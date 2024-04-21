#pragma once

#include <iosfwd>

namespace Http {
class Router;
}

namespace Signup {

struct SignupController {
    static void initialize(const std::string& prefix, Http::Router& router);
};

} // namespace Signup