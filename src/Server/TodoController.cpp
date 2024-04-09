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
        auto data = Data::AsJson::fromRecord(*record);
        auto my_array = nlohmann::json::array();

        for (auto i : record->listAsPointers()) {
            auto one_todo = nlohmann::json::object();
            one_todo["id"] = i->key();
            one_todo["selected"] = "";
            one_todo["description"] = i->values()["description"];
            my_array.push_back(one_todo);
        }
        data["todos"] = my_array;
        return content(BaseTemplate(TEMPLATE_DIR "/todo/edit.html")
                           .render(data))
            ->appendNavBarAction({"Start", "/"})
            .shared_from_this();
    } else {
        return recordNotFound(prefix(), record->presentableName());
    }
}
