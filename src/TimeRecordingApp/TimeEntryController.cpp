#include "TimeEntryController.hpp"

#include "Http/Request.hpp"
#include "Http/Response.hpp"
#include "Http/Router.hpp"
#include "Http/Session.hpp"
#include "Submit.hpp"
#include "TimeEntry.hpp"
using Http::redirect;

struct TimeEntryController::TimeEntryControllerImpl {
    string prefix;
};

std::shared_ptr<Http::Response> TimeEntryController::createEntry(
    const Request& request)
{
    using Http::Session;
    const auto prefix = impl_->prefix;

    auto entry = std::make_shared<TimeEntry>(request);
    for (const auto& field : entry->fields()) {
        if (request.hasParameter(field)) {
            entry->setImpl(field, request.parameter(field));
        }
        if (field == "user_id") {
            entry->setImpl("user_id", Session(request).userId());
        }
    }
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
shared_ptr<Http::Response> TimeEntryController::entryForm(
    const TimeEntryController::Request& request)
{
    using namespace Input;
    auto entry = make_shared<TimeEntry>(request);
    return Http::content(Form(*entry, impl_->prefix + "/", "post")
                       .appendElement(make_shared<Submit>(
                           "Create " + entry->presentableName()))())
        ->title("Create " + entry->presentableName())
        .shared_from_this();
}
TimeEntryController::~TimeEntryController() = default;
