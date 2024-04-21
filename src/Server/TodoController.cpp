#include "TodoController.hpp"

#include "Data/AsJson.hpp"
#include "Data/Record.hpp"
#include "Request.hpp"
#include "Response.hpp"
#include "Template/BaseTemplate.hpp"

#include <sstream>
#include <utility>

using Http::content;
using BaseTemplate = Template::BaseTemplate;

TodoController::TodoController(
    const string& prefix, make_record_func makeRecordFunc)
    : CrudController(prefix, std::move(makeRecordFunc))
{
}
std::shared_ptr<CrudController::Response> TodoController::editRecord(
    const Request& request)
{
    auto record = m_makeRecord(request);
    if (record->pop(request.query())) {
        auto data = Data::AsJson::fromRecord(*record);
        auto my_array = nlohmann::json::array();

        data["none_selected"] = "selected";
        for (const auto& i : record->listAsPointers()) {
            if (i->key() == record->key()) {
                continue;
            }
            bool selected = i->key() == record->values()["parent_id"];
            if (selected) {
                data["none_selected"] = "";
            }
            auto one_todo = nlohmann::json::object();
            one_todo["id"] = i->key();
            one_todo["selected"] = selected ? "selected" : "";
            one_todo["description"] = i->values()["description"];
            my_array.push_back(one_todo);
        }
        data["todos"] = my_array;
        return content(
                   BaseTemplate(TEMPLATE_DIR "/todo/edit.html").render(data))
            ->appendNavBarAction({"Start", "/"})
            .shared_from_this();
    } else {
        return recordNotFound(prefix(), record->presentableName());
    }
}

std::string recurseTodos(const std::vector<std::shared_ptr<Record>>& list, const std::string& parent_id = ""){
    // filter list for parent_id
    std::vector<std::shared_ptr<Record>> filtered;
    for (const auto& i:list) {
        if (i->values()["parent_id"] != parent_id) {
                continue;
        }
        filtered.push_back(i);
    }

    if (list.empty()) {
        return {};
    }
    std::ostringstream str;
    str << "<ul>";
    for (const auto& i:filtered) {
        str << "<li>";
        str << i->values()["description"];
        str << recurseTodos(list, i->key());
        str << "</li>";
    }
    str << "</ul>";
    return str.str();
}

shared_ptr<CrudController::Response> TodoController::listRecords(
    const CrudController::Request& request)
{

    auto record = m_makeRecord(request);
    return content(recurseTodos(record->listAsPointers()))
        ->appendAction(
            {"Create new " + record->presentableName(), prefix() + "/new"})
        .appendNavBarAction({"Start", "/"})
        .title(record->presentableName() + " List")
        .shared_from_this();

    // return CrudController::listRecords(request);
    // Go through records selected by parent_id = ""
    // For each record, go through records selected by parent_id = record->key()
    // Recursively
    //  generate <ul> nested list
}
