#pragma once

#include <Poco/Util/ServerApplication.h>

struct LoginController;
namespace Html {
struct Presentation;
}
namespace Http {
class Router;
}

class TimeRecordingApplication : public Poco::Util::ServerApplication {
public:
    static std::shared_ptr<LoginController> makeLoginController(
        Http::Router& router, std::shared_ptr<Html::Presentation> presentation);

protected:
    int main(const std::vector<std::string>& args) override;
};
