
#include "TimeRecordingApplication.hpp"

#include "Http/Session.hpp"
#include "Impl/PocoWebServer.hpp"
#include "Login/LoginController.hpp"
#include "Login/ProfileController.hpp"
#include "SimpleWebServer.hpp"
#include "TimeEntryController.hpp"
#include "User/PasswordChangeController.hpp"

int TimeRecordingApplication::main(const std::vector<std::string>& args)
{
    PocoWebServer httpServer;
    auto presentation = std::make_shared<Presentation>();

    auto privateHandler = std::make_shared<SimpleWebServer>();
    auto& privateRouter = privateHandler->router();
    auto timeEntryController
        = std::make_shared<TimeEntryController>("/time_entry")
              ->initialize(privateRouter)
              .shared_from_this();

    privateRouter.get("/", [](const Request& request) {
        return Http::redirect("/time_entry/");
    });
    PasswordChangeController::initialize("/password", privateRouter);
    std::make_shared<Login::ProfileController>()->initialize(privateRouter);
    privateHandler->defaultHandler(Http::NotFoundHandler);
    privateHandler->finish_init();

    auto controller = std::make_shared<LoginController>(
                          privateHandler, nullptr, nullptr, presentation)
                          ->initialize(httpServer.router())
                          .shared_from_this();

    controller->setPostProcessingHook([](const Request& request,
                                         const shared_ptr<Response>& response) {
        using Http::Session;
        if (Session(request).isAdmin()) {
#ifdef ENABLE_USER_LIST
            response->appendNavBarAction({"Users", "/user/", "right"});
#endif
            response->appendNavBarAction({"Sessions", "/sessions", "right"});
        }
        return response->appendNavBarAction({"🚪 Logout", "/logout", "right"})
            .appendNavBarAction(
                {"👤 " + Session(request).userName(), "/profile/", "right"})
            .shared_from_this();
    });

    httpServer.defaultHandler(controller->getDefaultHandler());
    httpServer.setPresentation(presentation);
    httpServer.finish_init();
    httpServer.start();
    waitForTerminationRequest();
    httpServer.stop();
    return EXIT_OK;
}
