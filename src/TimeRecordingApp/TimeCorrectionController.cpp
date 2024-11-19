
#include "TimeCorrectionController.hpp"

#include "Data/ValidationError.hpp"
#include "DateTime/Date.hpp"
#include "DateTime/Month.hpp"
#include "DateTime/Time.hpp"
#include "Html/List.hpp"
#include "Http/Request.hpp"
#include "Http/Response.hpp"
#include "Http/Router.hpp"
#include "Http/Session.hpp"
#include "Input/Date.h"
#include "Input/Submit.hpp"
#include "Input/Text.hpp"
#include "Input/TimeElement.hpp"
#include "String/currentDateTime.hpp"
#include "String/escape.hpp"
#include "Template/BaseTemplate.hpp"
#include "Template/TemplateDataHelper.hpp"
#include "TimeEntry.hpp"

using Http::content;
using Http::redirect;
using Http::Request;
using Http::Response;
using Http::Router;
using Http::Session;
using BaseTemplate = Template::BaseTemplate;

struct TimeCorrectionController::TimeCorrectionControllerImpl {
    string prefix;
};
TimeCorrectionController::~TimeCorrectionController() = default;
TimeCorrectionController::TimeCorrectionController(const string& prefix)
    : impl_(std::make_unique<TimeCorrectionControllerImpl>())
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
    router.get(prefix + "/edit", [ptr](const Request& request) {
        return ptr->editEntry(request);
    });
    router.post(prefix + "/update", [ptr](const Request& request) {
        return ptr->updateEntry(request);
    });
    router.get(prefix + "/new", [ptr](const Request& request) {
        return ptr->newEntry(request);
    });
    router.post(prefix + "/create", [ptr](const Request& request) {
        return ptr->createEntry(request);
    });
    return *this;
}
std::shared_ptr<Response> TimeCorrectionController::listEntries(
    const TimeCorrectionController::Request& request)
{
    if (!isAllowed(request)) {
        return content("Access denied");
    }
    auto record = make_shared<TimeEntry>(request);
    // Select by User
    // Check if Empty -> display Empty Message
    // Select Years
    // Select Months
    const auto user_id = Session(request).userId();

    spdlog::debug("User ID: {}", user_id);
    if (record->isEmptyFor(user_id)) {
        return content("Noch keine Einträge")
            ->title("Übersicht")
            .shared_from_this();
    }
    auto years = record->yearsFor(user_id);
    static const map<int, string> month_names
        = {{1, "Januar"},
           {2, "Februar"},
           {3, "März"},
           {4, "April"},
           {5, "Mai"},
           {6, "Juni"},
           {7, "Juli"},
           {8, "August"},
           {9, "September"},
           {10, "Oktober"},
           {11, "November"},
           {12, "Dezember"}};
    int selected_year = selectYear(request, years);
    spdlog::debug("Selected Year: {}", selected_year);
    const auto months = record->monthsFor(user_id, selected_year);

    int selected_month = selectMonth(request, months);

    spdlog::debug("Selected Month: {}", selected_month);

    auto years_with_selection
        = Template::selectFromList(years, selected_year, "year");

    auto months_with_selection
        = Template::selectFromList(months, selected_month, "month");
    for (auto& month : months_with_selection) {
        month["month_name"] = DateTime::Month(month["month"]).asString();
    }
    using DateTime::Date;

    auto result = record->overviewAsPointers(
        user_id,
        selected_year,
        selected_month,
        Date::currentDate().formatAsDate());
    // result should also provide
    // getHours
    // getTime("start")
    // getTime("end")
    // getDate("date")
    // to avoid parsing and unparsing several times internally
    // Calculate total hours!
    // day: Montag
    // date: 1. Juni
    // start_time: 10:00
    // end_time: 12:33
    // total_hours: 2:33
    auto data = nlohmann::json::object();
    data["years"] = years_with_selection;
    data["selected_year"] = selected_year;
    data["months"] = months_with_selection;
    if (result.empty()) {
        return content(BaseTemplate(TEMPLATE_DIR "/timeentry/list_empty.html")
                           .render(data))
            ->title("Übersicht")
            .shared_from_this();
    }
    using DateTime::Time;
    auto total_hours = Time(0, 0);
    data["rows"] = convertResultToData(result, total_hours);
    data["total_hours"] = total_hours.formatAsTotalHours();
    return content(
               BaseTemplate(TEMPLATE_DIR "/timeentry/list.html").render(data))
        ->title("Übersicht")
        .shared_from_this();
}
nlohmann::json TimeCorrectionController::convertResultToData(
    const vector<shared_ptr<Record>>& records, DateTime::Time& total)
{
    auto columns = records[0]->fields();
    auto rows = nlohmann::json::array();
    using DateTime::Time;
    for (const auto& entry : records) {
        auto row = nlohmann::json::object();
        const auto values = entry->values();
        for (const auto& field : columns) {
            row[field] = values.at(field);
        }
        const auto start_time = values.at("start_time");
        const auto end_time = values.at("end_time");
        if (start_time.empty() || end_time.empty()) {
            row["hours"] = "";
        } else {
            row["hours"] = Time::parseTime(end_time)
                               .difference(Time::parseTime(start_time))
                               .formatAsTotalHours();
        }
        if (!end_time.empty()) {
            auto difference = Time::parseTime(end_time).difference(
                Time::parseTime(start_time));
            total.add(difference);
        }
        row["start_id"] = values.at("start_id");
        row["end_id"] = values.at("end_id");
        row["enable_edit"] = !end_time.empty();
        row["note"] = String::escape(values.at("note"));
        row["show_note"] = !values.at("note").empty();
        rows.push_back(row);
    }
    return rows;
}
std::shared_ptr<Response> TimeCorrectionController::editEntry(
    const Request& request)
{
    if (!isAllowed(request)) {
        return content("Access denied");
    }
    /*
     * TODO: Testcases
     *
     * Need to provide time with leading zero! otherwise will not show
     */
    const auto prefix = impl_->prefix;
    auto data = nlohmann::json::object();
    const auto query = request.query();
    auto id_start = query.substr(0, 36);
    auto id_end = query.substr(37, 72);

    auto entry_start = make_shared<TimeEntry>(request);
    auto entry_end = make_shared<TimeEntry>(request);
    if (!entry_start->pop(id_start)) {
        return redirect(prefix + "/")
            ->alert("Invalid Start ID", Html::AlertType::DANGER)
            .shared_from_this();
    }
    if (!entry_end->pop(id_end)) {
        return redirect(prefix + "/")
            ->alert("Invalid End ID", Html::AlertType::DANGER)
            .shared_from_this();
    }
    if (entry_start->get("employee_id") != entry_end->get("employee_id")) {
        return redirect(prefix + "/")
            ->alert("Different User IDs", Html::AlertType::DANGER)
            .shared_from_this();
    }
    if (entry_start->get("employee_id") != Session(request).userId()) {
        return redirect(prefix + "/")
            ->alert("User ID not allowed to edit", Html::AlertType::DANGER)
            .shared_from_this();
    }
    if (entry_start->get("event_date") != entry_end->get("event_date")) {
        return redirect(prefix + "/")
            ->alert("Different Dates", Html::AlertType::DANGER)
            .shared_from_this();
    }
    using DateTime::Date;
    data["id"] = "";
    data["query"] = String::escape(query);
    data["id_start"] = String::escape(id_start);
    data["id_end"] = String::escape(id_end);
    data["day"] = Date(entry_start->getEventDate()).formatAsWeekday();
    data["date"] = Date(entry_start->getEventDate()).formatAsDayMonth();
    data["start_time"] = entry_start->get("event_time");
    data["end_time"] = entry_end->get("event_time");
    data["note"] = String::escape(entry_end->get("note"));
    return content(
               BaseTemplate(TEMPLATE_DIR "/timeentry/edit.html").render(data))
        ->title("Eintrag bearbeiten")
        .shared_from_this();
}
std::shared_ptr<Response> TimeCorrectionController::updateEntry(
    const Request& request)
{
    if (!isAllowed(request)) {
        return content("Access denied");
    }
    /*
     * Form parameters:
     * id_start
     * id_end
     * start_time
     * end_time
     */
    const auto expected_parameters
        = {"id_start", "id_end", "start_time", "end_time", "note"};
    const auto prefix = impl_->prefix;
    for (const auto& parameter : expected_parameters) {
        if (!request.hasParameter(parameter)) {
            return redirect(prefix + "/")
                ->alert(
                    string{"Missing parameter: "} + parameter,
                    Html::AlertType::DANGER)
                .shared_from_this();
        }
    }
    const auto id_start = request.parameter("id_start");
    const auto id_end = request.parameter("id_end");
    auto entry_start = make_shared<TimeEntry>(request);
    auto entry_end = make_shared<TimeEntry>(request);
    if (!entry_start->pop(id_start)) {
        return redirect(prefix + "/")
            ->alert("Invalid Start ID", Html::AlertType::DANGER)
            .shared_from_this();
    }
    if (!entry_end->pop(id_end)) {
        return redirect(prefix + "/")
            ->alert("Invalid End ID", Html::AlertType::DANGER)
            .shared_from_this();
    }
    if (entry_start->get("employee_id") != entry_end->get("employee_id")) {
        return redirect(prefix + "/")
            ->alert("Different User IDs", Html::AlertType::DANGER)
            .shared_from_this();
    }
    if (entry_start->get("employee_id") != Session(request).userId()) {
        return redirect(prefix + "/")
            ->alert("User ID not allowed to edit", Html::AlertType::DANGER)
            .shared_from_this();
    }
    if (entry_start->get("event_date") != entry_end->get("event_date")) {
        return redirect(prefix + "/")
            ->alert("Different Dates", Html::AlertType::DANGER)
            .shared_from_this();
    }
    // TODO: above checks are duplicated in editEntry
    /*
     * Where is the start_time parameter parsed and verified?
     */
    auto start_time = request.parameter("start_time");
    auto end_time = request.parameter("end_time");
    // Parse Times and check if end_time > start_time
    using DateTime::Time;
    auto difference
        = Time::parseTime(end_time).difference(Time::parseTime(start_time));
    if (difference.toMinutes() < 0) {
        return redirect(prefix + "/")
            ->alert(
                "Fehler: Stop kann nicht vor Start sein",
                Html::AlertType::DANGER)
            .shared_from_this();
    }
    // Also check for Overlaps with existing entries from the same day
    entry_start->set("event_time", start_time);
    entry_end->set("event_time", end_time);
    auto note = request.parameter("note");
    entry_end->set("note", note);
    entry_start->update();
    entry_end->update();
    // Apply month and year to list
    // get month as int from event_date
    const auto month = entry_start->getEventDate().month();
    const auto year = entry_start->getEventDate().year();
    return redirect(
               prefix + "/?year=" + std::to_string(year)
               + "&month=" + std::to_string(month))
        ->alert("Updated", Html::AlertType::SUCCESS)
        .shared_from_this();
}
int TimeCorrectionController::selectYear(
    const Request& request, const vector<int>& years)
{
    if (!request.hasParameter("year")) {
        return years.back();
    }
    auto selected_year = std::atoi(request.parameter("year").c_str());
    if (std::count(years.begin(), years.end(), selected_year) > 0) {
        return selected_year;
    }
    return years.back();
}
int TimeCorrectionController::selectMonth(
    const Request& request, const vector<int>& months)
{
    if (!request.hasParameter("month")) {
        return months.back();
    }
    auto selected_month = std::atoi(request.parameter("month").c_str());
    if (std::count(months.begin(), months.end(), selected_month) > 0) {
        return selected_month;
    }
    return months.back();
}
bool TimeCorrectionController::isAllowed(const Request& request)
{
    using Http::Session;
    Session session(request);
    return session.role() == "user";
}
std::shared_ptr<Response> TimeCorrectionController::newEntry(
    const Request& request)
{
    if (!isAllowed(request)) {
        return content("Access denied");
    }
    using namespace Input;
    const auto prefix = impl_->prefix;
    auto form = std::make_shared<Form>(
        vector<ElementPtr>{
            std::make_shared<Date>("date"),
            std::make_shared<TimeElement>("start"),
            std::make_shared<TimeElement>("end"),
            std::make_shared<Text>("note"),
            std::make_shared<Submit>("Create Entry")},
        prefix + "/create",
        "post");

    return content((*form)())
        ->form(form)
        .title("Neuer Eintrag")
        .shared_from_this();
}
std::shared_ptr<Response> TimeCorrectionController::createEntry(
    const Request& request)
{
    if (!isAllowed(request)) {
        return content("Access denied");
    }
    const auto prefix = impl_->prefix;

    auto entry_start = make_shared<TimeEntry>(request);
    auto entry_end = make_shared<TimeEntry>(request);
    try {
        const auto user_id = Session(request).userId();
        for (const std::string& parameter : {"date", "start", "end", "note"}) {
            if (!request.hasParameter(parameter)) {
                throw Data::ValidationError("Missing parameter: " + parameter);
            }
        }
        const auto event_date = request.parameter("date");
        const auto start_time = request.parameter("start");
        const auto end_time = request.parameter("end");
        entry_start->set("employee_id", user_id);
        entry_start->set("event_date", event_date); // TODO: validate date
        entry_start->set("event_time", start_time);
        entry_start->set("event_type", "start");
        entry_start->set("corrected_event_id", "");
        entry_start->set("deleted_event_id", "");
        entry_start->set("creation_date", String::localDateTime());
        entry_start->set("creator_user_id", user_id);
        entry_start->set("note", request.parameter("note"));
        entry_end->set("employee_id", user_id);
        entry_end->set("event_date", event_date); // TODO: validate date
        entry_end->set("event_time", end_time);
        entry_end->set("event_type", "stop");
        entry_end->set("corrected_event_id", "");
        entry_end->set("deleted_event_id", "");
        entry_end->set("creation_date", String::localDateTime());
        entry_end->set("creator_user_id", user_id);
        entry_end->set("note", request.parameter("note"));
        if (entry_start->checkTimestampExists(user_id, event_date, start_time)||
            entry_end->checkTimestampExists(user_id, event_date, end_time)) {
            return redirect(prefix + "/")
                ->alert(
                    "Fehler: Zeitpunkt wurde bereits erfasst",
                    Html::AlertType::DANGER)
                .shared_from_this();
        }
        if (DateTime::Time::parseTime(end_time).difference(
                DateTime::Time::parseTime(start_time)).toMinutes() < 0) {
            return redirect(prefix + "/")
                ->alert(
                    "Fehler: Stop kann nicht vor Start sein",
                    Html::AlertType::DANGER)
                .shared_from_this();
        }
        // TODO Check if there is already an Entry in that same range
        entry_start->insert();
        entry_end->insert();
    } catch (const Data::ValidationError& ex) {
        return redirect(prefix + "/new")
            //TODO: what() contains the submitted data, could be exploited
            ->alert(ex.what(), Html::AlertType::DANGER)
            .shared_from_this();
    }
    return redirect(prefix + "/")
        ->alert("Created new Time Entry", Html::AlertType::SUCCESS)
        .shared_from_this();
}
