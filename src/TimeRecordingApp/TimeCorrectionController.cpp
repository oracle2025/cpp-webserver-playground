
#include "TimeCorrectionController.hpp"

#include "Html/List.hpp"
#include "Http/Request.hpp"
#include "Http/Response.hpp"
#include "Http/Router.hpp"
#include "TimeEntry.hpp"

using Http::content;
using Http::Request;
using Http::Response;
using Http::Router;

struct TimeCorrectionController::TimeCorrectionControllerImpl {
    string prefix;
};
TimeCorrectionController::~TimeCorrectionController() = default;
TimeCorrectionController::TimeCorrectionController(
    const TimeCorrectionController::string& prefix)
    : impl_(new TimeCorrectionControllerImpl)
{
    impl_->prefix = prefix;
}
TimeCorrectionController& TimeCorrectionController::initialize(
    Http::Router& router)
{
    const auto prefix = impl_->prefix;
    auto ptr = shared_from_this();

    router.get(prefix + "/", [ptr](const Request& request) {
        return ptr->listEntries(request);
    });
    return *this;
}
std::shared_ptr<Response> TimeCorrectionController::listEntries(
    const TimeCorrectionController::Request& request)
{
    auto record = make_shared<TimeEntry>(request);
    using namespace Input;
    auto columns = record->presentableFieldsImpl();
    return content(Html::List(record->listAsPointers(), columns)
                       .prefix(impl_->prefix)())
        ->title(record->presentableName() + " List")
        .shared_from_this();
}
