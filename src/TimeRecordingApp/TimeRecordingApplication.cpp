
#include "TimeRecordingApplication.hpp"

#include "Http/NullHandler.hpp"
#include "Http/Session.hpp"
#include "Impl/PocoWebServer.hpp"
#include "Login/LoginController.hpp"
#include "Login/ProfileController.hpp"
#include "Signup/SignupController.hpp"
#include "SimpleWebServer.hpp"
#include "TimeCorrectionController.hpp"
#include "TimeEntryController.hpp"
#include "User/PasswordChangeController.hpp"
#include "String/capitalize.hpp"

int TimeRecordingApplication::main(const std::vector<std::string>& args)
{
    PocoWebServer httpServer;
    auto presentation = std::make_shared<Presentation>("Time Tracking");

    auto privateHandler = std::make_shared<SimpleWebServer>();
    auto& privateRouter = privateHandler->router();
    auto timeEntryController
        = std::make_shared<TimeEntryController>("/time_entry")
              ->initialize(privateRouter)
              .shared_from_this();

    PasswordChangeController::initialize("/password", privateRouter);
    std::make_shared<Login::ProfileController>()->initialize(privateRouter);
    privateRouter.get("/", [](const Request& request) {
        return Http::redirect("/time_entry/");
    });

    std::make_shared<TimeCorrectionController>("/list")->initialize(
        privateRouter);

    privateHandler->defaultHandler(Http::NotFoundHandler);
    privateHandler->finish_init();

    shared_ptr<SimpleWebServer> publicHandler = nullptr;
#ifdef ENABLE_SIGNUP
    publicHandler = std::make_shared<SimpleWebServer>();
    Signup::SignupController::initialize("/signup", publicHandler->router());
    publicHandler->defaultHandler(Http::NullHandler);
    publicHandler->finish_init();
#endif

    auto controller = std::make_shared<LoginController>(
                          privateHandler, nullptr, publicHandler, presentation)
                          ->initialize(httpServer.router())
                          .shared_from_this();

    controller->setPostProcessingHook([](const Request& request,
                                         const shared_ptr<Response>& response) {
        using Http::Session;
        response->appendNavBarAction({"Übersicht", "/list/"});
        //response->appendNavBarAction({"Auswertung", "/report/"});
        if (Session(request).isAdmin()) {
#ifdef ENABLE_USER_LIST
            response->appendNavBarAction({"Users", "/user/", "right"});
#endif
            response->appendNavBarAction({"Sessions", "/sessions", "right"});
        }
        return response->appendNavBarAction({"🚪 Logout", "/logout", "right"})
            .appendNavBarAction(
                {"👤 " + String::capitalize(Session(request).userName()), "/profile/", "right"})
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
