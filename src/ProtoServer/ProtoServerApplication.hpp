#pragma once

#include <Poco/Util/ServerApplication.h>

namespace ProtoServer {

using std::string;
using std::vector;

class ProtoServerApplication : public Poco::Util::ServerApplication {
    int main(const vector<string>& args) override;

};

} // namespace ProtoServer
