
#include "TimeRecordingApplication.hpp"

#include "Impl/PocoWebServer.hpp"
#include "Login/LoginController.hpp"
#include "SimpleWebServer.hpp"
#include "TimeEntryController.hpp"

int TimeRecordingApplication::main(const std::vector<std::string>& args)
{
    PocoWebServer httpServer;
    auto presentation = std::make_shared<Presentation>();

    auto privateHandler = std::make_shared<SimpleWebServer>();
    auto timeEntryController = std::make_shared<TimeEntryController>(
                                   "/time_entry")
                                   ->initialize(privateHandler->router())
                                   .shared_from_this();
    
    privateHandler->router().get("/", [](const Request& request) {
        return Http::redirect("/time_entry/");
    });
    privateHandler->defaultHandler(Http::NotFoundHandler);
    privateHandler->finish_init();


    auto controller = std::make_shared<LoginController>(
                     privateHandler, nullptr, nullptr, presentation)
                     ->initialize(httpServer.router())
                     .shared_from_this();

    httpServer.defaultHandler(controller->getDefaultHandler());
    httpServer.setPresentation(presentation);
    httpServer.finish_init();
    httpServer.start();
    waitForTerminationRequest();
    httpServer.stop();
    return EXIT_OK;
}
