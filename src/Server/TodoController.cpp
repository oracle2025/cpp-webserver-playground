#include "TodoController.hpp"

#include "Data/AsJson.hpp"
#include "Data/Record.hpp"
#include "Input/CheckBoxSelect.hpp"
#include "Request.hpp"
#include "Response.hpp"
#include "String/escape.hpp"
#include "Template/BaseTemplate.hpp"
#include "doctest.h"

#include <spdlog/spdlog.h>

#include <sstream>
#include <utility>

using Http::content;
using BaseTemplate = Template::BaseTemplate;

struct HierarchicalList {
    HierarchicalList(const std::vector<std::shared_ptr<Record>>& list)
    {
        static const KeyStringType PARENT_ID_FIELD = "parent_id";
        for (const auto& i : list) {
            const auto parent_id = i->values()[PARENT_ID_FIELD];
            if (ordered.find(parent_id) == ordered.end()) {
                ordered[parent_id] = {i};
            } else {
                ordered[parent_id].push_back(i);
            }
        }
    }
    std::vector<std::shared_ptr<Record>> children(const string& parent_id) const
    {
        if (ordered.find(parent_id) == ordered.end()) {
            return {};
        }
        return ordered.at(parent_id);
    }
    std::map<std::string, std::vector<std::shared_ptr<Record>>> ordered;
};

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

struct RecursiveTodoRender {
    virtual ~RecursiveTodoRender() = default;
    virtual void tableStart(int level, const string& parent_id)
    {
        if (level > 0) {
            str << R"(<tr><td colspan="3" style="padding: 0px;">)"
                << R"(<table class="table" id="subitems-)" + parent_id
                    + "\">\n";
        } else {
            str << "<table class=\"table\">\n";
        }
    }
    virtual void tableEnd(int level)
    {
        str << "</table>\n";
        if (level > 0) {
            str << R"(</td></tr>)" << "\n";
        }
    }
    virtual void rowStart(
        int level,
        const string& key,
        const std::map<KeyStringType, string>& values)
    {
        str << R"(<tr><td class="max" width="99%">)" << "\n";
        for (int c = 0; c < level; c++) {
            str << "&nbsp;&nbsp;&nbsp;&nbsp;";
        }
        static const KeyStringType CHECKED_FIELD = "checked";
        static const KeyStringType DESCRIPTION_FIELD = "description";
        str << R"(<input type="hidden" name=")" << key << R"(" value="no" />)";
        if (values.at(CHECKED_FIELD) == "yes") {
            str << R"(<input type="checkbox" name=")" << key
                << R"(" checked value="yes" onchange="submitForm(this);">)"
                << "\n";
        } else {
            str << R"(<input type="checkbox" name=")" << key
                << R"(" value="yes" onchange="submitForm(this);">)" << "\n";
        }
        str << "&nbsp;&nbsp;&nbsp;&nbsp;";
        str << values.at(DESCRIPTION_FIELD);
    }
    virtual void editDeleteButtons(const string& key)
    {
        str << R"(</td><td><a href="/todo/edit?)" << key
            << R"(" class="edit button btn btn-success btn-sm">✏️ <span class="hidden-xs">Edit</span></a></td>
<td><a href="/todo/delete?)"
            << key
            << R"(" class="remove button btn btn-danger btn-sm">♻️ <span class="hidden-xs">Delete</span></a>)";
        str << "</td></tr>\n";
    }
    virtual void hideButton(const string& key)
    {
        str << R"( <button type="button" class="btn btn-link btn-sm mt-2 toggle-subitems" id="hide-)"
            << key << R"(">Hide&nbsp;subtasks</button>)" << "\n";
    }
    string result()
    {
        return str.str();
    }
    std::ostringstream str;
};

/* it may be simpler to just indent by number of parents
 * instead of using nested lists.
 * Table should give better formatting also*/
template<typename T>
std::string recurseTodos(
    const HierarchicalList& list,
    int level = 0,
    const std::string& parent_id = "")
{
    spdlog::debug("recurseTodos: level: {}, parent_id: {}", level, parent_id);
    // filter list for parent_id
    std::vector<std::shared_ptr<Record>> descendents = list.children(parent_id);
    // auto &descendents = list;

    if (descendents.empty()) {
        return {};
    }
    T renderer;
    renderer.tableStart(level, parent_id);
    for (const auto& i : descendents) {
        renderer.rowStart(level, i->key(), i->values());
        if (level == 0 && !list.children(i->key()).empty()) {
            renderer.hideButton(i->key());
        }
        renderer.editDeleteButtons(i->key());
        renderer.str << recurseTodos<T>(list, level + 1, i->key());
    }
    renderer.tableEnd(level);
    return renderer.result();
}

TEST_CASE("RecursiveTodoRendering")
{
    static const KeyStringType DESCRIPTION_FIELD = "description";
    static const KeyStringType CHECKED_FIELD = "checked";
    static const KeyStringType PARENT_ID_FIELD = "parent_id";
    static const KeyStringType KEY_FIELD = "key";
    struct ConcreteRecord : Record {
        ConcreteRecord(const string& key, const string& parent_id = {})
        {
            m_values[DESCRIPTION_FIELD] = key;
            m_values[CHECKED_FIELD] = "no";
            m_values[PARENT_ID_FIELD] = parent_id;
            m_values[KEY_FIELD] = key;
        }
        string key() const override
        {
            return m_values.at(KEY_FIELD);
        }
        std::vector<KeyStringType> fields() const override
        {
            return {DESCRIPTION_FIELD, CHECKED_FIELD, PARENT_ID_FIELD};
        }
        std::map<KeyStringType, string> values() const override
        {
            return m_values;
        }
        HtmlInputType inputType(const KeyStringType& field) const override
        {
            return HtmlInputType::TEXT;
        }
        string presentableName() const override
        {
            return "Todo";
        }
        std::map<KeyStringType, string> m_values;
    };
    std::vector<std::shared_ptr<Record>> list;
    list.push_back(std::make_shared<ConcreteRecord>("A"));
    list.push_back(std::make_shared<ConcreteRecord>("B"));
    list.push_back(std::make_shared<ConcreteRecord>("C"));
    list.push_back(std::make_shared<ConcreteRecord>("D", "A"));
    auto result = recurseTodos<RecursiveTodoRender>(list);
    CHECK_EQ(result, R"(<table class="table">
<tr><td class="max" width="99%">
<input type="hidden" name="A" value="no" /><input type="checkbox" name="A" value="yes" onchange="submitForm(this);">
&nbsp;&nbsp;&nbsp;&nbsp;A <button type="button" class="btn btn-link btn-sm mt-2 toggle-subitems" id="hide-A">Hide&nbsp;subtasks</button>
</td><td><a href="/todo/edit?A" class="edit button btn btn-success btn-sm">✏️ <span class="hidden-xs">Edit</span></a></td>
<td><a href="/todo/delete?A" class="remove button btn btn-danger btn-sm">♻️ <span class="hidden-xs">Delete</span></a></td></tr>
<tr><td colspan="3" style="padding: 0px;"><table class="table" id="subitems-A">
<tr><td class="max" width="99%">
&nbsp;&nbsp;&nbsp;&nbsp;<input type="hidden" name="D" value="no" /><input type="checkbox" name="D" value="yes" onchange="submitForm(this);">
&nbsp;&nbsp;&nbsp;&nbsp;D</td><td><a href="/todo/edit?D" class="edit button btn btn-success btn-sm">✏️ <span class="hidden-xs">Edit</span></a></td>
<td><a href="/todo/delete?D" class="remove button btn btn-danger btn-sm">♻️ <span class="hidden-xs">Delete</span></a></td></tr>
</table>
</td></tr>
<tr><td class="max" width="99%">
<input type="hidden" name="B" value="no" /><input type="checkbox" name="B" value="yes" onchange="submitForm(this);">
&nbsp;&nbsp;&nbsp;&nbsp;B</td><td><a href="/todo/edit?B" class="edit button btn btn-success btn-sm">✏️ <span class="hidden-xs">Edit</span></a></td>
<td><a href="/todo/delete?B" class="remove button btn btn-danger btn-sm">♻️ <span class="hidden-xs">Delete</span></a></td></tr>
<tr><td class="max" width="99%">
<input type="hidden" name="C" value="no" /><input type="checkbox" name="C" value="yes" onchange="submitForm(this);">
&nbsp;&nbsp;&nbsp;&nbsp;C</td><td><a href="/todo/edit?C" class="edit button btn btn-success btn-sm">✏️ <span class="hidden-xs">Edit</span></a></td>
<td><a href="/todo/delete?C" class="remove button btn btn-danger btn-sm">♻️ <span class="hidden-xs">Delete</span></a></td></tr>
</table>
)");
    struct SimpleRecursiveRenderer : public RecursiveTodoRender {
        void tableStart(int level, const string& parent_id) override
        {
            if (level > 0) {
                str << "Subtable Start: " << parent_id << "\n";
            } else {
                str << "Table Start\n";
            }
        }
        void tableEnd(int level) override
        {
            if (level > 0) {
                str << "Subtable End\n";
            } else {
                str << "Table End\n";
            }
        }
        void rowStart(
            int level,
            const string& key,
            const std::map<KeyStringType, string>& values) override
        {
            for (int c = 0; c < level; c++) {
                str << "\t";
            }
            str << "Row Start: " << key << " ";
            str << values.at(DESCRIPTION_FIELD);
        }
        void editDeleteButtons(const string& key) override
        {
            str << " Edit/Delete: " << key << "\n";
        }
        void hideButton(const string& key) override
        {
            str << " HideButton-" << key;
        }
    };
    for (int i = 0; i < 100; i++) {
        list.push_back(std::make_shared<ConcreteRecord>(std::to_string(i)));
    }
    auto result2 = recurseTodos<SimpleRecursiveRenderer>(list);
    CHECK_EQ(result2, R"(Table Start
Row Start: A A HideButton-A Edit/Delete: A
Subtable Start: A
	Row Start: D D Edit/Delete: D
Subtable End
Row Start: B B Edit/Delete: B
Row Start: C C Edit/Delete: C
Row Start: 0 0 Edit/Delete: 0
Row Start: 1 1 Edit/Delete: 1
Row Start: 2 2 Edit/Delete: 2
Row Start: 3 3 Edit/Delete: 3
Row Start: 4 4 Edit/Delete: 4
Row Start: 5 5 Edit/Delete: 5
Row Start: 6 6 Edit/Delete: 6
Row Start: 7 7 Edit/Delete: 7
Row Start: 8 8 Edit/Delete: 8
Row Start: 9 9 Edit/Delete: 9
Row Start: 10 10 Edit/Delete: 10
Row Start: 11 11 Edit/Delete: 11
Row Start: 12 12 Edit/Delete: 12
Row Start: 13 13 Edit/Delete: 13
Row Start: 14 14 Edit/Delete: 14
Row Start: 15 15 Edit/Delete: 15
Row Start: 16 16 Edit/Delete: 16
Row Start: 17 17 Edit/Delete: 17
Row Start: 18 18 Edit/Delete: 18
Row Start: 19 19 Edit/Delete: 19
Row Start: 20 20 Edit/Delete: 20
Row Start: 21 21 Edit/Delete: 21
Row Start: 22 22 Edit/Delete: 22
Row Start: 23 23 Edit/Delete: 23
Row Start: 24 24 Edit/Delete: 24
Row Start: 25 25 Edit/Delete: 25
Row Start: 26 26 Edit/Delete: 26
Row Start: 27 27 Edit/Delete: 27
Row Start: 28 28 Edit/Delete: 28
Row Start: 29 29 Edit/Delete: 29
Row Start: 30 30 Edit/Delete: 30
Row Start: 31 31 Edit/Delete: 31
Row Start: 32 32 Edit/Delete: 32
Row Start: 33 33 Edit/Delete: 33
Row Start: 34 34 Edit/Delete: 34
Row Start: 35 35 Edit/Delete: 35
Row Start: 36 36 Edit/Delete: 36
Row Start: 37 37 Edit/Delete: 37
Row Start: 38 38 Edit/Delete: 38
Row Start: 39 39 Edit/Delete: 39
Row Start: 40 40 Edit/Delete: 40
Row Start: 41 41 Edit/Delete: 41
Row Start: 42 42 Edit/Delete: 42
Row Start: 43 43 Edit/Delete: 43
Row Start: 44 44 Edit/Delete: 44
Row Start: 45 45 Edit/Delete: 45
Row Start: 46 46 Edit/Delete: 46
Row Start: 47 47 Edit/Delete: 47
Row Start: 48 48 Edit/Delete: 48
Row Start: 49 49 Edit/Delete: 49
Row Start: 50 50 Edit/Delete: 50
Row Start: 51 51 Edit/Delete: 51
Row Start: 52 52 Edit/Delete: 52
Row Start: 53 53 Edit/Delete: 53
Row Start: 54 54 Edit/Delete: 54
Row Start: 55 55 Edit/Delete: 55
Row Start: 56 56 Edit/Delete: 56
Row Start: 57 57 Edit/Delete: 57
Row Start: 58 58 Edit/Delete: 58
Row Start: 59 59 Edit/Delete: 59
Row Start: 60 60 Edit/Delete: 60
Row Start: 61 61 Edit/Delete: 61
Row Start: 62 62 Edit/Delete: 62
Row Start: 63 63 Edit/Delete: 63
Row Start: 64 64 Edit/Delete: 64
Row Start: 65 65 Edit/Delete: 65
Row Start: 66 66 Edit/Delete: 66
Row Start: 67 67 Edit/Delete: 67
Row Start: 68 68 Edit/Delete: 68
Row Start: 69 69 Edit/Delete: 69
Row Start: 70 70 Edit/Delete: 70
Row Start: 71 71 Edit/Delete: 71
Row Start: 72 72 Edit/Delete: 72
Row Start: 73 73 Edit/Delete: 73
Row Start: 74 74 Edit/Delete: 74
Row Start: 75 75 Edit/Delete: 75
Row Start: 76 76 Edit/Delete: 76
Row Start: 77 77 Edit/Delete: 77
Row Start: 78 78 Edit/Delete: 78
Row Start: 79 79 Edit/Delete: 79
Row Start: 80 80 Edit/Delete: 80
Row Start: 81 81 Edit/Delete: 81
Row Start: 82 82 Edit/Delete: 82
Row Start: 83 83 Edit/Delete: 83
Row Start: 84 84 Edit/Delete: 84
Row Start: 85 85 Edit/Delete: 85
Row Start: 86 86 Edit/Delete: 86
Row Start: 87 87 Edit/Delete: 87
Row Start: 88 88 Edit/Delete: 88
Row Start: 89 89 Edit/Delete: 89
Row Start: 90 90 Edit/Delete: 90
Row Start: 91 91 Edit/Delete: 91
Row Start: 92 92 Edit/Delete: 92
Row Start: 93 93 Edit/Delete: 93
Row Start: 94 94 Edit/Delete: 94
Row Start: 95 95 Edit/Delete: 95
Row Start: 96 96 Edit/Delete: 96
Row Start: 97 97 Edit/Delete: 97
Row Start: 98 98 Edit/Delete: 98
Row Start: 99 99 Edit/Delete: 99
Table End
)");
}

shared_ptr<CrudController::Response> TodoController::listRecords(
    const CrudController::Request& request)
{

    auto record = m_makeRecord(request);
    using namespace Input;
    return content(
               Form(
                   /*R"(<table class="table">)" + */
                   recurseTodos<RecursiveTodoRender>(record->listAsPointers())
                   /*+ "</table>"*/,
                   prefix() + "/mark",
                   "post")())
        ->appendAction(
            {"Create new " + record->presentableName(), prefix() + "/new"})
        .title(record->presentableName() + " List")
        .shared_from_this();

    // return CrudController::listRecords(request);
    // Go through records selected by parent_id = ""
    // For each record, go through records selected by parent_id =
    // record->key() Recursively
    //  generate <ul> nested list
}
