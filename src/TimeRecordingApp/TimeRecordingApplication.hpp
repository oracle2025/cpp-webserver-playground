#pragma once

#include <Poco/Util/ServerApplication.h>

class TimeRecordingApplication : public Poco::Util::ServerApplication {
protected:
    int main(const std::vector<std::string>& args) override;
};
