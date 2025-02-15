
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
#include "doctest.h"

#include <chrono>
#include <string>

using Http::content;
using Http::Request;
using Http::Response;

int calculate_rowspan(int month_starts_at_weekday_index, int days_in_month)
{
    return (month_starts_at_weekday_index + days_in_month) / 7;
}

struct TimeReportController::TimeReportControllerImpl {
    std::string prefix;
};

TimeReportController::~TimeReportController() = default;
TimeReportController::TimeReportController(const std::string& prefix)
    : impl_(std::make_unique<TimeReportControllerImpl>())
{
    impl_->prefix = prefix;
}
std::ostream& operator<<(std::ostream& os, const date::year_month_day& ymd)
{
    return os << static_cast<int>(ymd.year()) << '-' << std::setw(2)
              << std::setfill('0') << static_cast<unsigned>(ymd.month()) << '-'
              << std::setw(2) << static_cast<unsigned>(ymd.day());
}
TimeReportController& TimeReportController::initialize(Http::Router& router)
{
    const auto prefix = impl_->prefix;
    auto ptr = shared_from_this();
    router.get(prefix + "/", [ptr](const Request& request) {
        return ptr->list(request);
    });
    router.get(prefix + "/calendar", [ptr](const Request& request) {
        /* Iterate over all days in the year from 1.1.2025 to 31.12.2025 */
        // date::year_month_day date{date::May/23/2021};
        //  Source:
        //  https://www.reddit.com/r/cpp_questions/comments/vrktgx/iterating_over_days_dates_in_c_20/
        date::sys_days days = date::January / 1 / 2025;
        auto end = days + date::days{365};
        std::ostringstream str;
        str << "<table border='1' class='yearly_calendar'>";
        str << "<tr><td></td><td>Mo</td><td>Di</td><td>Mi</td><td>Do</"
               "td><td>Fr</td><td>Sa</td><td>So</td><td></td></tr>";
        str << "<tr><th rowspan=\"4\">Jan</th>";
        /*
         * January:
         * Starts on Wednesday Index[2]
         * January: rowspan = 4
         * Has 31 Days
         * +---+--+--+--+--+--+--+--+
         * |   |Mo|Di|Mi|Do|Fr|Sa|So|
         * |Jan|  |  |01|02|03|04|05|
         * |   |06|07|08|09|10|11|12|
         * |   |13|14|15|16|17|18|19|
         * |   |20|21|22|23|24|25|26|
         * |Feb|27|28|29|30|31|  |  |
         * +---+--+--+--+--+--+--+--+
         *
         * Edge Cases: Month starts on Monday
         * Month ends on Sunday
         * February has 28 days
         * February has 29 days
         *
         * rowspan = (month_starts_at_weekday_index + days_in_month) / 7
         *
         *
         */
        const date::year_month_weekday& ymwd_first_row = days;
        for (const auto& i : std::vector<unsigned>{1, 2, 3, 4, 5, 6, 0}) {
            if (date::weekday{i} == ymwd_first_row.weekday()) {
                break;
            }
            str << "<td></td>";
        }
        for (; days < end; days += date::days{1}) {
            const date::year_month_day& ymd = days;
            const date::year_month_weekday& ymwd = days;
            if (ymwd.weekday() == date::weekday{6}
                || ymwd.weekday() == date::weekday{0}) {
                str << "<th>" << ymd.day() << "</th>";
                /*
                 * Use style classes for CSS Formatting:
                 * class="weekend"
                 * class="first row of Month"
                 * class="first Day of Month"
                 */
            } else {
                str << "<td>" << ymd.day() << "</td>";
            }
            if (ymwd.weekday() == date::weekday{0}) {
                std::ostringstream debug;

                int int_year = static_cast<int>(ymd.year());

                unsigned int int_month = static_cast<unsigned>(ymd.month());
                const int days_of_month
                    = Poco::DateTime::daysOfMonth(int_year, int_month);

                date::month m = ymd.month();
                m++;
                int_month = static_cast<unsigned>(m);
                // year month 1
                Poco::DateTime d(int_year, int_month, 1);

                auto dayOfWeek = d.dayOfWeek() - 1;
                while (dayOfWeek < 0) {
                    dayOfWeek += 7;
                }

                const auto rowspan = calculate_rowspan(
                    dayOfWeek,
                    Poco::DateTime::daysOfMonth(

                        int_year, int_month));
                debug << "(" << m << ": d.dayOfWeek():" << d.dayOfWeek()
                      << " days_of_month:"
                      << Poco::DateTime::daysOfMonth(int_year, int_month)
                      << " rowspan: " << rowspan << " " << dayOfWeek << ")";
                if (days_of_month - static_cast<unsigned>(ymd.day()) < 7) {
                    auto m = ymd.month();
                    m++;
                    str << "<td>&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&"
                           "nbsp;&"
                           "nbsp;&nbsp;&nbsp;&nbsp;</td></tr><tr>"
                        << "<th rowspan=\"" << rowspan << "\">" << m;
                    /*<< " rowspan=" << rowspan
                        << " year=" << int_year << " month= " << int_month
                        << " days_of_month=" << days_of_month << " " <<
                       debug.str() <<*/
                    str << " </th>";
                } else {
                    str << "<td>&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&"
                           "nbsp;&"
                           "nbsp;&nbsp;&nbsp;&nbsp;</td></tr><tr>";
                    //<< "<td>" << ymd.month() << "</td>";
                }
            }
            // str << days << "<br>\n";
            /* Todo: calculate rowspan
             * <tr>
             *  <td rowspan="5">January</td>
             *  <td>1</td>
             *  <td>2</td>
             *  <td>3</td>
             *  <td>4</td>
             *  <td>5</td>
             *  <td>6</td>
             *  <td>7</td>
             *  <td>Start of the Year</td>
             * </tr>
             */

            /*if (isSunday(days)) {
                nextRow();
            }
            if (isLastDayOfMonth(days)) {
                nextMonth();
            }*/
        }
        str << "</tr></table>";
        return content("Hello Calendar:<br>\n" + str.str())
            ->title("Report")
            .shared_from_this();
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

    auto months_with_selection
        = Template::selectFromList(months, selected_month, "month");
    for (auto& month : months_with_selection) {
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

    return content(
               Template::BaseTemplate(TEMPLATE_DIR "/timeentry/report.html")
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

TEST_CASE("Calendar")
{
    /*
     * +---+--+--+--+--+--+--+--+
     * |   |Mo|Di|Mi|Do|Fr|Sa|So|
     * |Jan|  |  |01|02|03|04|05|
     * |   |06|07|08|09|10|11|12|
     * |   |13|14|15|16|17|18|19|
     * |   |20|21|22|23|24|25|26|
     * |Feb|27|28|29|30|31|  |  |
     * +---+--+--+--+--+--+--+--+
     * Expectation: rowspan = 4
     *
     * month_starts_at_weekday_index = 2
     * days_in_month = 31
     */
    SUBCASE("January 2025 starts at wednesday")
    {
        date::sys_days days = date::January / 1 / 2025;
        const date::year_month_weekday& ymwd_first_row = days;
        auto month_starts_at_weekday_index
            = ymwd_first_row.weekday().c_encoding();
        /* 0 == sunday
         * 1 == monday
         * 2 == tuesday
         * 3 == wednesday
         * 4 == thursday
         * 5 == friday
         * 6 == saturday
         */
        CHECK(month_starts_at_weekday_index == 3);
    }

    SUBCASE("Calculate Rowspan from starting index and days in month")
    {
        const int days_in_month = 31;
        const int month_starts_at_weekday_index = 3;
        const int rowspan
            = calculate_rowspan(month_starts_at_weekday_index, days_in_month);
        CHECK(rowspan == 4);
    }

    /*
     * +---+--+--+--+--+--+--+--+
     * |   |Mo|Di|Mi|Do|Fr|Sa|So|
     * |Jan|01|02|03|04|05|06|07|
     * |   |08|09|10|11|12|13|14|
     * |   |15|16|17|18|19|20|21|
     * |   |22|23|24|25|26|27|28|
     * |Feb|29|30|31|  |  |  |  |
     * +---+--+--+--+--+--+--+--+
     * Expectation: rowspan = 4
     *
     * month_starts_at_weekday_index = 0
     * days_in_month = 31
     */

    SUBCASE("Calculate Rowspan for Month starting on Monday")
    {
        const int days_in_month = 31;
        const int month_starts_at_weekday_index = 0;
        const int rowspan
            = calculate_rowspan(month_starts_at_weekday_index, days_in_month);
        CHECK(rowspan == 4);
    }

    /*
     * +---+--+--+--+--+--+--+--+
     * |   |Mo|Di|Mi|Do|Fr|Sa|So|
     * |Jan|  |  |  |  |  |01|02|
     * |   |03|04|05|06|07|08|09|
     * |   |10|11|12|13|14|15|16|
     * |   |17|18|19|20|21|22|23|
     * |   |24|25|26|27|28|30|31|
     * +---+--+--+--+--+--+--+--+
     * Expectation: rowspan = 5
     *
     * month_starts_at_weekday_index = 5
     * days_in_month = 31
     */
    SUBCASE("Calculate Rowspan for Month starting on Saturday")
    {
        const int days_in_month = 31;
        const int month_starts_at_weekday_index = 5;
        const int rowspan
            = calculate_rowspan(month_starts_at_weekday_index, days_in_month);
        CHECK(rowspan == 5);
    }

    /*
     * +---+--+--+--+--+--+--+--+
     * |   |Mo|Di|Mi|Do|Fr|Sa|So|
     * |Jan|  |  |  |  |  |01|02|
     */
}