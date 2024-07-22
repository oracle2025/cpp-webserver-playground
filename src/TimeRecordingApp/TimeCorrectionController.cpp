
#include "TimeCorrectionController.hpp"

#include "Html/List.hpp"
#include "Http/Request.hpp"
#include "Http/Response.hpp"
#include "Http/Router.hpp"
#include "Http/Session.hpp"
#include "String/currentDateTime.hpp"
#include "String/timeDifference.hpp"
#include "Template/BaseTemplate.hpp"
#include "Time/Time.hpp"
#include "TimeEntry.hpp"

using Http::content;
using Http::Request;
using Http::Response;
using Http::Router;
using Http::Session;
using BaseTemplate = Template::BaseTemplate;

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
    using Http::redirect;

    router.get(prefix + "/", [ptr](const Request& request) {
        return ptr->listEntries(request);
    });
    router.get(prefix + "/enable_debug", [prefix](const Request& request) {
        spdlog::set_level(spdlog::level::debug);
        return redirect(prefix + "/")
            ->alert("Log Level Debug enabled", Html::AlertType::SUCCESS)
            .shared_from_this();
    });
    return *this;
}
std::shared_ptr<Response> TimeCorrectionController::listEntries(
    const TimeCorrectionController::Request& request)
{
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
    vector<int> months;
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
    int selected_year = 0;
    if (request.hasParameter("year")
        && std::count(
               years.begin(),
               years.end(),
               std::atoi(request.parameter("year").c_str()))
            > 0) {
        selected_year = std::atoi(request.parameter("year").c_str());
        months = record->monthsFor(user_id, selected_year);
    } else {
        selected_year = years.back();
        months = record->monthsFor(user_id, selected_year);
    }
    spdlog::debug("Selected Year: {}", selected_year);

    int selected_month = 0;
    if (request.hasParameter("month")
        && std::count(
               months.begin(),
               months.end(),
               std::atoi(request.parameter("month").c_str()))
            > 0) {
        selected_month = std::atoi(request.parameter("month").c_str());
    } else {
        selected_month = months.back();
    }
    spdlog::debug("Selected Month: {}", selected_month);

    nlohmann::json::array_t years_with_selection;
    for (auto year : years) {
        nlohmann::json year_with_selection;
        year_with_selection["year"] = year;
        if (year == selected_year) {
            year_with_selection["selected"] = true;
        } else {
            year_with_selection["selected"] = false;
        }
        years_with_selection.push_back(year_with_selection);
    }

    nlohmann::json::array_t months_with_selection;
    for (auto month : months) {
        if (month_names.find(month) == month_names.end()) {
            continue;
        }
        nlohmann::json month_with_selection;
        month_with_selection["month"] = month_names.at(month);
        month_with_selection["month_number"] = month;
        if (month == selected_month) {
            month_with_selection["selected"] = true;
        } else {
            month_with_selection["selected"] = false;
        }
        months_with_selection.push_back(month_with_selection);
    }

    auto result
        = record->overviewAsPointers(
        user_id, selected_year, selected_month, String::currentDate());
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
    auto columns = result[0]->fields();
    auto rows = nlohmann::json::array();
    using Time::Time;
    auto total_hours = Time(0, 0);
    for (const auto& entry : result) {
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
            row["hours"] = String::timeDifference(start_time, end_time);
        }
        if (!end_time.empty()) {
            auto difference = Time::parseTime(end_time).difference(
                Time::parseTime(start_time));
            total_hours.add(difference);
        }
        rows.push_back(row);
    }

    data["rows"] = rows;
    data["total_hours"] = total_hours.toString();
    return content(
               BaseTemplate(TEMPLATE_DIR "/timeentry/list.html").render(data))
        ->title("Übersicht")
        .shared_from_this();
}
