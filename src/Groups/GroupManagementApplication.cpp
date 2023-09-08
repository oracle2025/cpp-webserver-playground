#include "GroupManagementApplication.hpp"
#include "Http/RequestDispatcher.hpp"
#include "SimpleWebServer.hpp"
#include "Impl/PocoWebServer.hpp"
#include "Login/LoginController.hpp"
#include "Signup/SignupController.hpp"

namespace Groups {

using std::make_shared;
using Http::RequestDispatcher;
using Http::RequestHandlerList;

int GroupManagementApplication::main(const vector<string>& args)
{
   /* auto handlers = RequestHandlerList{
        make_shared<TodoController<SimpleWebServer>>("/todo"),
        make_shared<SchluesselmanagementController<SimpleWebServer>>("/schluesselmanagement"),
        make_shared<BenutzerController<SimpleWebServer>>("/benutzer"),
        make_shared<MitarbeiterController<SimpleWebServer>>("/mitarbeiter"),
        make_shared<ElternController<SimpleWebServer>>("/eltern"),
        make_shared<KinderController<SimpleWebServer>>("/kinder"),
        make_shared<LieferantenController<SimpleWebServer>>("/lieferanten"),
        make_shared<WaescheDienstController<SimpleWebServer>>("/waeschedienst"),
        make_shared<GelieheneBuecherController<SimpleWebServer>>("/geliehenebuecher")};
    LoginController<PocoWebServer> server(
        make_shared<RequestDispatcher>(handlers),
        make_shared<Signup::SignupController<SimpleWebServer>>("/signup"),
        std::make_shared<Presentation>());
    server.start();
    waitForTerminationRequest();
    server.stop();*/
    return EXIT_OK;
}
} // namespace Groups