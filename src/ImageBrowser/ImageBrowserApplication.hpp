#pragma once

#include <Poco/Util/ServerApplication.h>

#include <string>
#include <vector>
using std::string;
using std::vector;
namespace ImageBrowser {

class ImageBrowserApplication : public Poco::Util::ServerApplication {
protected:
    int main(const vector<string>& args) override;
};

} // namespace ImageBrowser
