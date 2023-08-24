#pragma once

#include <Poco/Util/ServerApplication.h>

namespace Groups {

using std::string;
using std::vector;

class GroupManagementApplication : public Poco::Util::ServerApplication {
protected:
    int main(const vector<string>& args) override;
};

} // namespace Groups
