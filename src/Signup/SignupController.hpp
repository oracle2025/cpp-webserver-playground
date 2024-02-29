#pragma once

#include <iosfwd>

namespace Http {
class Router;
}

namespace Signup {

struct SignupController {
    explicit SignupController(const std::string& prefix, Http::Router& router);
};

} // namespace Signup