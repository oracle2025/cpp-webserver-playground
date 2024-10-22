#pragma once

#include <iosfwd>

namespace Http {
class Router;
}
namespace Calendar {

struct CalendarController {
    static void initialize(const std::string& prefix, Http::Router& router);
};

} // namespace Calendar
