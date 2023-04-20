#pragma once

#include <Poco/Util/ServerApplication.h>
#include <string>
#include <vector>

using std::string;
using std::vector;

class LoginServerApplication : public Poco::Util::ServerApplication {
protected:
    int main(const vector<string> &args) override;
};
