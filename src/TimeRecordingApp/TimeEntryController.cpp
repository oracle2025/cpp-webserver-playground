#include "TimeEntryController.hpp"

#include "Http/Request.hpp"
#include "Http/Response.hpp"
#include "Http/Router.hpp"
#include "Http/Session.hpp"
#include "String/capitalize.hpp"
#include "String/currentDateTime.hpp"
#include "Submit.hpp"
#include "Template/BaseTemplate.hpp"
#include "TimeEntry.hpp"

using Http::content;
using Http::redirect;

using BaseTemplate = Template::BaseTemplate;

struct TimeEntryController::TimeEntryControllerImpl {
    string prefix;
};
shared_ptr<Http::Response> TimeEntryController::entryForm(
    const TimeEntryController::Request& request)
{
    using namespace Input;
    using Http::Session;
    auto entry = make_shared<TimeEntry>(request);
    auto data = nlohmann::json::object();

    data["Username"] = String::capitalize(Session(request).userName());

    return content(
        BaseTemplate(TEMPLATE_DIR "/timeentry/new.html").render(data));

    /* return Http::content(Form(*entry, impl_->prefix + "/", "post")
                              .appendElement(make_shared<Submit>(
                                  "Create " + entry->presentableName()))())
         ->title("Create " + entry->presentableName())
         .shared_from_this();*/
}
std::shared_ptr<Http::Response> TimeEntryController::createEntry(
    const Request& request)
{
    using Http::Session;
    const auto prefix = impl_->prefix;
    auto entry = std::make_shared<TimeEntry>(request);
    const auto user_id = Session(request).userId();
    if(!request.hasParameter("event_time")){
        return redirect(prefix + "/")
            ->alert("Please enter a time", Html::AlertType::DANGER)
            .shared_from_this();
    }
    if(!request.hasParameter("event_type")){
        return redirect(prefix + "/")
            ->alert("Please enter an event type", Html::AlertType::DANGER)
            .shared_from_this();
    }
    entry->set("employee_id", user_id);
    entry->set("event_date", String::currentDate());
    entry->set("event_time", request.parameter("event_time"));
    entry->set("event_type", request.parameter("event_type"));
    entry->set("corrected_event_id", "");
    entry->set("deleted_event_id", "");
    entry->set("creation_date", String::localDateTime());
    entry->set("creator_user_id", user_id);
    entry->insert();
    return redirect(prefix + "/")
        ->alert(entry->presentableName() + " created", Html::AlertType::SUCCESS)
        .shared_from_this();
}
TimeEntryController& TimeEntryController::initialize(Http::Router& router)
{
    const auto prefix = impl_->prefix;
    auto ptr = shared_from_this();

    router.get(prefix + "/", [ptr](const Request& request) {
        return ptr->entryForm(request);
    });
    router.post(prefix + "/", [ptr](const Request& request) {
        return ptr->createEntry(request);
    });
    return *this;
}
TimeEntryController::TimeEntryController(const string& prefix)
    : impl_(new TimeEntryControllerImpl)
{
    impl_->prefix = prefix;
}

TimeEntryController::~TimeEntryController() = default;
