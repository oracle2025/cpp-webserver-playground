
#include "TimeReportController.hpp"

#include "Data/User.hpp"
#include "DateTime/Date.hpp"
#include "DateTime/Time.hpp"
#include "Http/Response.hpp"
#include "Http/Router.hpp"
#include "String/capitalize.hpp"
#include "Template/BaseTemplate.hpp"
#include "TimeCorrectionController.hpp"
#include "TimeEntry.hpp"

#include <string>

using Http::content;
using Http::Request;
using Http::Response;

struct TimeReportController::TimeReportControllerImpl {
    std::string prefix;
};

TimeReportController::~TimeReportController() = default;
TimeReportController::TimeReportController(const std::string& prefix)
    : impl_(std::make_unique<TimeReportControllerImpl>())
{
    impl_->prefix = prefix;
}
TimeReportController& TimeReportController::initialize(Http::Router& router)
{
    const auto prefix = impl_->prefix;
    auto ptr = shared_from_this();
    router.get(prefix + "/", [ptr](const Request& request) {
        return ptr->list(request);
    });
    return *this;
}
std::shared_ptr<Response> TimeReportController::list(const Request& request)
{ /* Select Users */
    /* Select Month */
    /* Select hours for Month */
    auto record = make_shared<TimeEntry>(request);

    auto years = record->yearsForAllUsers();
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
        months = record->monthsForAllUsers(selected_year);
    } else {
        selected_year = years.back();
        months = record->monthsForAllUsers(selected_year);
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
    // Select all users where role == employee
    // Select all time entries for selected user
    // Generate View without EDIT button
    auto users = Data::findUsersByRole("user");
    auto users_json = nlohmann::json::array();
    for (const auto& user : users) {

        using DateTime::Date;
        auto time_entries = record->overviewAsPointers(
            user->key(),
            selected_year,
            selected_month,
            Date::currentDate().formatAsDate());
        using DateTime::Time;
        auto total_hours = Time(0, 0);
        auto rows = time_entries.empty()
            ? nlohmann::json::array()
            : TimeCorrectionController::convertResultToData(
                  time_entries, total_hours);

        nlohmann::json user_json;
        user_json["user_id"] = user->key();
        user_json["username"] = user->get("username");
        user_json["name"] = String::capitalize(user->get("username"));
        user_json["rows"] = rows;
        user_json["total_hours"] = total_hours.formatAsTotalHours();
        spdlog::debug("Rows: {}", rows.dump());
        users_json.push_back(user_json);
    }
    auto data = nlohmann::json::object();
    data["years"] = years_with_selection;
    data["selected_year"] = selected_year;
    data["months"] = months_with_selection;
    data["users"] = users_json;
    spdlog::debug("Years: {}", data["years"].dump());

    return content(Template::BaseTemplate(TEMPLATE_DIR "/timeentry/report.html")
                       .render(data))
        ->title("Report")
        .shared_from_this();
}
