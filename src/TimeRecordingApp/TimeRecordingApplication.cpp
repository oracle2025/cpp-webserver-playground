
#include "TimeRecordingApplication.hpp"

#include "Http/NullHandler.hpp"
#include "Http/Session.hpp"
#include "Impl/PocoWebServer.hpp"
#include "Login/LoginController.hpp"
#include "Login/ProfileController.hpp"
#include "Signup/SignupController.hpp"
#include "SimpleWebServer.hpp"
#include "String/capitalize.hpp"
#include "TimeCorrectionController.hpp"
#include "TimeEntryController.hpp"
#include "TimeReportController.hpp"
#include "User/PasswordChangeController.hpp"
#include "User/UserAdminController.hpp"

std::shared_ptr<LoginController> TimeRecordingApplication::makeLoginController(
    Router& router, std::shared_ptr<Html::Presentation> presentation)
{
    auto privateHandler = std::make_shared<SimpleWebServer>();
    auto& privateRouter = privateHandler->router();
    auto timeEntryController
        = std::make_shared<TimeEntryController>("/time_entry")
              ->initialize(privateRouter)
              .shared_from_this();

    PasswordChangeController::initialize("/password", privateRouter);
    std::make_shared<Login::ProfileController>()->initialize(privateRouter);
    privateRouter.get("/", [](const Request& request) {
        using Http::Session;
        Session session(request);
        if (session.role() == "user") {
            return Http::redirect("/time_entry/");
        } else {
            return Http::redirect("/report/");
        }
    });
    privateRouter.get("/enable_debug", [](const Request& request) {
        spdlog::set_level(spdlog::level::debug);
        using Http::redirect;
        return redirect("/")
            ->alert("Log Level Debug enabled", Html::AlertType::SUCCESS)
            .shared_from_this();
    });

    std::make_shared<TimeCorrectionController>("/list")->initialize(
        privateRouter);

    std::make_shared<TimeReportController>("/report")->initialize(
        privateRouter);

    for (const auto& [id, handler] : privateRouter) {
        spdlog::debug("Route: " + id.path());
    }

    privateHandler->defaultHandler(Http::NotFoundHandler);
    privateHandler->finish_init();

    shared_ptr<SimpleWebServer> adminHandler = nullptr;
#ifdef ENABLE_USER_LIST
    adminHandler = make_shared<SimpleWebServer>();
    auto userCrud = std::make_shared<UserAdminController>(
                        "/user",
                        [](const Request& request) {
                            return std::make_shared<Data::User>(request);
                        })
                        ->initialize(adminHandler->router())
                        .shared_from_this();
    adminHandler->defaultHandler(Http::NullHandler);
    adminHandler->finish_init();
#endif
    shared_ptr<SimpleWebServer> publicHandler = nullptr;
#ifdef ENABLE_SIGNUP
    publicHandler = std::make_shared<SimpleWebServer>();
    Signup::SignupController::initialize("/signup", publicHandler->router());
    publicHandler->defaultHandler(Http::NullHandler);
    publicHandler->finish_init();
#endif

    auto controller
        = std::make_shared<LoginController>(
              privateHandler, adminHandler, publicHandler, presentation)
              ->initialize(router)
              .shared_from_this();

    controller->setPostProcessingHook([](const Request& request,
                                         const shared_ptr<Response>& response) {
        using Http::Session;
        Session session(request);
        if (session.role() == "user") {
            response->appendNavBarAction({"Übersicht", "/list/"});
        } else {
            response->appendNavBarAction({"Auswertung", "/report/"});
        }
        if (session.isAdmin()) {
#ifdef ENABLE_USER_LIST
            response->appendNavBarAction({"Users", "/user/", "right"});
#endif
            response->appendNavBarAction({"Sessions", "/sessions", "right"});
            response->appendNavBarAction(
                {"Role",
                 "/role",
                 "right",
                 {{"Admin", "/role/?admin"},
                  {"Team Member", "/role/?teammember"},
                  {"Book Keeper", "/role/?bookkeeper"}}});
        }
        return response->appendNavBarAction({"🚪 Logout", "/logout", "right"})
            .appendNavBarAction(
                {"👤 " + String::capitalize(Session(request).userName()),
                 "/profile/",
                 "right"})
            .shared_from_this();
    });

    return controller;
}

int TimeRecordingApplication::main(const std::vector<std::string>& args)
{
    PocoWebServer httpServer;
    httpServer.setServerPort(SERVER_PORT_TIME_RECORDING);
    auto presentation = std::make_shared<Presentation>("Time Tracking");
    auto controller = makeLoginController(httpServer.router(), presentation);

    httpServer.defaultHandler(controller->getDefaultHandler());
    httpServer.setPresentation(presentation);
    httpServer.finish_init();
    httpServer.start();
    waitForTerminationRequest();
    httpServer.stop();
    return EXIT_OK;
}
