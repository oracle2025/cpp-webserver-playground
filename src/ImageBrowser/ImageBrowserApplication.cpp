
#include "ImageBrowserApplication.hpp"

#include "ImageBrowserController.hpp"
#include "Impl/PocoWebServer.hpp"

namespace ImageBrowser {
int ImageBrowserApplication::main(const vector<string>& args)
{
    ImageBrowserController<PocoWebServer> server("/imagebrowser");
    server.start();
    waitForTerminationRequest();
    server.stop();
    return EXIT_OK;
}
} // namespace ImageBrowser