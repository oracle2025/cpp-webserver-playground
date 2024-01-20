#include "LoginServerApplication.hpp"

#include "Data/Event.hpp"
#include "Email/SendEmailController.hpp"
#include "Events/CalendarController.hpp"
#include "Filter/ByOwner.hpp"
#include "Http/RequestDispatcher.hpp"
#include "Impl/PocoWebServer.hpp"
#include "LoginController.hpp"
#include "Signup/SignupController.hpp"
#include "User/PasswordChangeController.hpp"
#include "doctest.h"

using Email::SendEmailController;
using Events::CalendarController;
using Http::RequestDispatcher;
using Http::RequestHandlerList;
using std::make_shared;

int LoginServerApplication::main(const vector<string>& args)
{
    Data::Event event_items{Http::Request{}};
    std::ifstream input_stream(CONFIG_DIR "/events.csv");

    // check stream status
    if (input_stream) {
        event_items.initFromCsv(input_stream);
    } else {
        std::istringstream csv(R"(subject,startDate,endDate,startTime,endTime
First of January,2021-01-01,2021-01-01,00:00,23:59
First of February,2021-02-01,2021-02-01,00:00,23:59
First of March,2021-03-01,2021-03-01,00:00,23:59
First of April,2021-04-01,2021-04-01,00:00,23:59
)");
        event_items.initFromCsv(csv);
    }

    // Router router;
    // auto shared_todo_list = make_shared<CrudController<SimpleWebServer,
    // Todo>>("/shared", router);
    // #error shared todo list not implemented
    auto handler = std::make_shared<SimpleWebServer>();
    auto makeOwnerTodoRecord = [](const Http::Request& request) {
        return std::make_shared<Filter::ByOwner>(request);
    };
    CrudController todoCrud(
        "/todo",
        [](const Request& request) {
            return std::make_shared<Filter::ByOwner>(request);
        },
        handler->router());
    CrudController eventCrud(
        "/event",
        [](const Request& request) {
            return std::make_shared<Data::Event>(request);
        },
        handler->router());
    handler->defaultHandler(Http::NullHandler);
    handler->finish_init();

    auto secretHandlers = RequestHandlerList{
        handler,
        make_shared<PasswordChangeController<SimpleWebServer>>("/password"),
        make_shared<CalendarController<SimpleWebServer>>("/calendar"),
        make_shared<SendEmailController<SimpleWebServer>>("/email"),
    };
    shared_ptr<SimpleWebServer> adminHandler = nullptr;
#ifdef ENABLE_USER_LIST
    adminHandler = make_shared<SimpleWebServer>();
    CrudController userCrud(
        "/user",
        [](const Request& request) {
            return std::make_shared<Data::User>(request);
        },
        adminHandler->router());
    adminHandler->defaultHandler(Http::NullHandler);
    adminHandler->finish_init();
#endif
    shared_ptr<RequestHandler> publicHandler = nullptr;
#ifdef ENABLE_SIGNUP
    publicHandler
        = make_shared<Signup::SignupController<SimpleWebServer>>("/signup");
#endif
    LoginController<PocoWebServer> server(
        make_shared<RequestDispatcher>(secretHandlers),
        adminHandler,
        publicHandler,
        std::make_shared<Presentation>());

    // server.addButtonBar({"Calendar", "Events", "Todo", "Users", "Tickets"});

    server.start();
    waitForTerminationRequest();
    server.stop();
    return EXIT_OK;
}
