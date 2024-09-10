#include "CrudController.hpp"

#include "Confirm.hpp"
#include "Form.hpp"
#include "Http/Request.hpp"
#include "Http/Response.hpp"
#include "Http/Session.hpp"
#include "List.hpp"
#include "Router.hpp"
#include "Submit.hpp"
#include "style.hpp"

using Http::content;
using Http::redirect;
using Http::Response;

struct CrudController::CrudControllerImpl {
    string prefix;
};

CrudController::CrudController(
    const string& prefix, make_record_func makeRecordFunc)
    : m_makeRecord(std::move(makeRecordFunc))
    , impl_(std::make_unique<CrudControllerImpl>())
{
    impl_->prefix = prefix;
}
CrudController &CrudController::initialize(Http::Router& router)
{
    const auto prefix = impl_->prefix;
    auto ptr = shared_from_this();
    router.get(prefix + "/new", [ptr, prefix](const Request& request) {
        using namespace Input;
        auto record = ptr->m_makeRecord(request);
        return content(Form(*record, prefix + "/create", "post")
                           .appendElement(make_shared<Submit>(
                               "Create " + record->presentableName()))())
            ->title("Create " + record->presentableName())
            .shared_from_this();
    });
    router.post(prefix + "/create", [ptr, prefix](const Request& request) {
        using Http::Session;
        auto record = ptr->m_makeRecord(request);
        for (const auto& field : record->fields()) {
            if (request.hasParameter(field)) {
                record->setImpl(field, request.parameter(field));
            }
            if (field == "user_id") {
                record->setImpl("user_id", Session(request).userId());
            }
        }
        record->insert();
        return redirect(prefix + "/edit?" + record->key())
            ->alert(
                record->presentableName() + " created",
                Html::AlertType::SUCCESS)
            .shared_from_this();
    });
    router.get(prefix + "/edit", [ptr](const Request& request) {
        return ptr->editRecord(request);
    });
    router.post(prefix + "/update", [ptr, prefix](const Request& request) {
        auto record = ptr->m_makeRecord(request);
        if (record->pop(request.query())) {
            for (auto i : record->fields()) {
                if (request.hasParameter(i)) {
                    record->setImpl(i, request.parameter(i));
                }
            }
            record->update();
            return redirect(prefix + "/edit?" + record->key())
                ->alert(
                    record->presentableName() + " updated",
                    Html::AlertType::SUCCESS)
                .shared_from_this();
        } else {
            return recordNotFound(prefix, record->presentableName());
        }
    });
    router.post(prefix + "/mark", [ptr, prefix](const Request& request) {
        auto record = ptr->m_makeRecord(request);
        std::ostringstream str;
        for (const auto& [key, value] : request.allParameters()) {
            record->pop(key);
            if (record->getImpl("checked") != value) {
                str << record->getImpl("description") << " is now "
                    << (value == "yes" ? "checked" : "unchecked") << std::endl;
            }
            record->setImpl("checked", value);
            record->update();
        }
        return redirect(prefix + "/")
            ->alert("Todo " + str.str(), Html::AlertType::SUCCESS)
            .shared_from_this();
    });
    router.post(prefix + "/delete", [ptr, prefix](const Request& request) {
        auto record = ptr->m_makeRecord(request);
        if (record->pop(request.query())) {
            if (request.hasParameter("confirmed")) {
                record->erase();
                return redirect(prefix + "/")
                    ->alert("Todo deleted", Html::AlertType::WARNING)
                    .shared_from_this();
            } else if (request.hasParameter("canceled")) {
                return redirect(prefix + "/")
                    ->alert("Delete canceled", Html::AlertType::INFO)
                    .shared_from_this();
            } else {
                return redirect(prefix + "/confirm?" + record->key())
                    ->alert(
                        "Are you sure you want to delete this todo?",
                        Html::AlertType::WARNING)
                    .shared_from_this();
            }
        } else {
            return recordNotFound(prefix, record->presentableName());
        }
    });
    router.get(prefix + "/delete", [ptr, prefix](const Request& request) {
        auto record = ptr->m_makeRecord(request);
        if (record->pop(request.query())) {
            return redirect(prefix + "/confirm?" + record->key())
                ->alert(
                    "Are you sure you want to delete this "
                    "todo?",
                    Html::AlertType::WARNING)
                .shared_from_this();
        } else {
            return recordNotFound(prefix, record->presentableName());
        }
    });
    router.get(prefix + "/confirm", [ptr, prefix](const Request& request) {
        using namespace Input;
        auto record = ptr->m_makeRecord(request);
        if (record->pop(request.query())) {
            return Confirm(prefix, *record, record->descriptionImpl())()
                ;
        } else {
            return recordNotFound(prefix, record->presentableName());
        }
    });
    router.get(prefix + "/", [ptr, prefix](const Request& request) {
        return ptr->listRecords(request);
    });
    if (!prefix.empty()) {
        router.get("/", [prefix](const Request& request) {
            return redirect(prefix + "/");
        });
    }
    router.get("/css/style.css", [](const Request& request) {
        return content(STYLE_SHEET, "text/css");
    });
    // T::defaultHandler(Http::NullHandler);
    // T::finish_init();
    return *this;
}
CrudController::~CrudController() = default;

shared_ptr<Response> CrudController::recordNotFound(
    const string& prefix, const string& presentableName)
{
    return content(presentableName + " not found")
        ->code(Response::NOT_FOUND)
        .shared_from_this();
}
const string& CrudController::prefix() const
{
    return impl_->prefix;
}
std::shared_ptr<Response> CrudController::editRecord(const Request& request)
{
    using namespace Input;
    auto record = m_makeRecord(request);
    if (record->pop(request.query())) {
        return content(Form(
                           *record,
                           string(prefix() + "/update?") + record->key(),
                           "post")
                           .appendElement(make_shared<Submit>(
                               "Update " + record->presentableName()))())
            ->title("Edit " + record->presentableName())
            .shared_from_this();
    } else {
        return recordNotFound(prefix(), record->presentableName());
    }
}
std::shared_ptr<Response> CrudController::listRecords(const Request& request)
{
    auto record = m_makeRecord(request);
    using namespace Input;
    auto columns = record->presentableFieldsImpl();
    return content(Form(
                       Html::List(record->listAsPointers(), columns)
                           .prefix(prefix())(),
                       prefix() + "/mark",
                       "post")())
        ->appendAction(
            {"Create new " + record->presentableName(), prefix() + "/new"})
        .title(record->presentableName() + " List")
        .shared_from_this();
}
