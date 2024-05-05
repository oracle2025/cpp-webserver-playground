#pragma once

#include <memory>
namespace Http {
class Router;
}
namespace Login {

class ProfileController
    : public std::enable_shared_from_this<ProfileController> {
public:
    ProfileController& initialize(Http::Router& router);
};

} // namespace Login
