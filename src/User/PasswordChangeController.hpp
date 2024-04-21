#pragma once

#include <iosfwd>

using std::string;
namespace Http {
class Router;
}
struct PasswordChangeController {
    static void initialize(const string& prefix, Http::Router& router);
};