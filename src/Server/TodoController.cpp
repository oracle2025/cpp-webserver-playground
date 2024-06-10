#include "TodoController.hpp"

#include "Data/AsJson.hpp"
#include "Data/Record.hpp"
#include "Input/CheckBoxSelect.hpp"
#include "Request.hpp"
#include "Response.hpp"
#include "String/escape.hpp"
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
            if (!i->values()["parent_id"].empty()) {
                continue;
            }
            bool selected = i->key() == record->values()["parent_id"];
            if (selected) {
                data["none_selected"] = "";
            }
            auto one_todo = nlohmann::json::object();
            one_todo["id"] = i->key();
            one_todo["selected"] = selected ? "selected" : "";
            one_todo["description"]
                = String::escape(i->values()["description"]);
            my_array.push_back(one_todo);
        }
        data["todos"] = my_array;
        return content(
            BaseTemplate(TEMPLATE_DIR "/todo/edit.html").render(data));
    } else {
        return recordNotFound(prefix(), record->presentableName());
    }
}

std::vector<std::shared_ptr<Record>> filtered(
    const std::vector<std::shared_ptr<Record>>& list,
    const std::string& parent_id)
{
    std::vector<std::shared_ptr<Record>> result;
    for (const auto& i : list) {
        if (i->values()["parent_id"] != parent_id) {
            continue;
        }
        result.push_back(i);
    }
    return result;
}

/* it may be simpler to just indent by number of parents
 * instead of using nested lists.
 * Table should give better formatting also*/
std::string recurseTodos(
    const std::vector<std::shared_ptr<Record>>& list,
    int level = 0,
    const std::string& parent_id = "")
{
    // filter list for parent_id
    std::vector<std::shared_ptr<Record>> descendents
        = filtered(list, parent_id);

    if (descendents.empty()) {
        return {};
    }
    std::ostringstream str;
    if (level > 0) {
        str << R"(<tr><td colspan="3" style="padding: 0px;">)";
        str << R"(<table class="table" id="subitems-)" << parent_id << "\">\n";
    } else {
        str << "<table class=\"table\">\n";
    }
    for (const auto& i : descendents) {
        str << R"(<tr><td class="max" width="99%">)" << "\n";
        for (int c = 0; c < level; c++) {
            str << "&nbsp;&nbsp;&nbsp;&nbsp;";
        }
        str << R"(<input type="hidden" name=")" << i->key()
            << R"(" value="no" />)";
        if (i->values()["checked"] == "yes") {
            str << R"(<input type="checkbox" name=")" << i->key()
                << R"(" checked value="yes" onchange="submitForm(this);">)"
                << "\n";
        } else {
            str << R"(<input type="checkbox" name=")" << i->key()
                << R"(" value="yes" onchange="submitForm(this);">)" << "\n";
        }
        str << "&nbsp;&nbsp;&nbsp;&nbsp;";
        str << i->values()["description"];
        if (level == 0 && !filtered(list, i->key()).empty()) {
            str << R"( <button type="button" class="btn btn-link btn-sm mt-2 toggle-subitems" id="hide-)"
                << i->key() << R"(">Hide&nbsp;subtasks</button>)" << "\n";
        }
        str << R"(</td><td><a href="/todo/edit?)" << i->key()
            << R"(" class="edit button btn btn-success btn-sm">✏️ <span class="hidden-xs">Edit</span></a></td>
<td><a href="/todo/delete?)"
            << i->key()
            << R"(" class="remove button btn btn-danger btn-sm">♻️ <span class="hidden-xs">Delete</span></a>)";
        str << "</td></tr>\n";
        str << recurseTodos(list, level + 1, i->key());
    }
    str << "</table>\n";
    if (level > 0) {
        str << R"(</td></tr>)" << "\n";
    }
    return str.str();
}

shared_ptr<CrudController::Response> TodoController::listRecords(
    const CrudController::Request& request)
{

    auto record = m_makeRecord(request);
    using namespace Input;
    return content(Form(
                       /*R"(<table class="table">)" + */
                       recurseTodos(record->listAsPointers())
                       /*+ "</table>"*/,
                       prefix() + "/mark",
                       "post")())
        ->appendAction(
            {"Create new " + record->presentableName(), prefix() + "/new"})
        .title(record->presentableName() + " List")
        .shared_from_this();

    // return CrudController::listRecords(request);
    // Go through records selected by parent_id = ""
    // For each record, go through records selected by parent_id = record->key()
    // Recursively
    //  generate <ul> nested list
}
