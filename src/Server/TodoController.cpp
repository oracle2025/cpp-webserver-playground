#include "TodoController.hpp"

#include <utility>

#include "Data/AsJson.hpp"
#include "Data/Record.hpp"
#include "Request.hpp"
#include "Response.hpp"
#include "Template/BaseTemplate.hpp"

using Http::content;
using BaseTemplate = Template::BaseTemplate;

TodoController::TodoController(
    const string& prefix, make_record_func makeRecordFunc, Http::Router& router)
    : CrudController(prefix, std::move(makeRecordFunc), router)
{
}
std::shared_ptr<CrudController::Response> TodoController::editRecord(
    const Request& request)
{
    auto record = m_makeRecord(request);
    if (record->pop(request.query())) {
        return content(BaseTemplate(TEMPLATE_DIR "/todo/edit.html")
                           .render(Data::AsJson::fromRecord(*record)))
            ->appendNavBarAction({"Start", "/"})
            .shared_from_this();
    } else {
        return recordNotFound(prefix(), record->presentableName());
    }
}
