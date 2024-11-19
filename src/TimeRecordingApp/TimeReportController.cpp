
#include "TimeReportController.hpp"

#include "Data/User.hpp"
#include "DateTime/Date.hpp"
#include "DateTime/Month.hpp"
#include "DateTime/Time.hpp"
#include "Http/Response.hpp"
#include "Http/Router.hpp"
#include "Http/Session.hpp"
#include "String/capitalize.hpp"
#include "Template/BaseTemplate.hpp"
#include "Template/TemplateDataHelper.hpp"
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

    using Http::Session;
    Session session(request);
    if ((session.role() != "employer") && (!session.isAdmin())) {
        return content("Access denied");
    }

    // TODO if current_user.role != employer return permission denied
    auto record = make_shared<TimeEntry>(request);

    const auto years = record->yearsForAllUsers();
    int selected_year = selectYear(request, years);
    const auto months = record->monthsForAllUsers(selected_year);
    spdlog::debug("Selected Year: {}", selected_year);

    int selected_month = selectMonth(request, months);
    spdlog::debug("Selected Month: {}", selected_month);

    auto years_with_selection
        = Template::selectFromList(years, selected_year, "year");

    auto months_with_selection = Template::selectFromList(
        months, selected_month, "month");
    for (auto &month : months_with_selection) {
        month["month_name"] = DateTime::Month(month["month"]).asString();
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
int TimeReportController::selectYear(
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
int TimeReportController::selectMonth(
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
