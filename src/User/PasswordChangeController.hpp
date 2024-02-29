#pragma once

#include <iosfwd>

using std::string;
namespace Http {
class Router;
}
struct PasswordChangeController {
    PasswordChangeController(const string& prefix, Http::Router& router);
};