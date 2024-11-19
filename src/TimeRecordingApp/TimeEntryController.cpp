#include "TimeEntryController.hpp"

#include "Data/ValidationError.hpp"
#include "DateTime/Date.hpp"
#include "DateTime/Time.hpp"
#include "Http/Request.hpp"
#include "Http/Response.hpp"
#include "Http/Router.hpp"
#include "Http/Session.hpp"
#include "Input/Submit.hpp"
#include "String/capitalize.hpp"
#include "String/currentDateTime.hpp"
#include "String/escape.hpp"
#include "Template/BaseTemplate.hpp"
#include "Text.hpp"
#include "TimeEntry.hpp"

using Http::content;
using Http::redirect;

using BaseTemplate = Template::BaseTemplate;

struct TimeEntryController::TimeEntryControllerImpl {
    string prefix;
};
shared_ptr<Http::Response> TimeEntryController::entryForm(
    const TimeEntryController::Request& request)
{
    if (!isAllowed(request)) {
        return content("Access denied");
    }
    using namespace Input;
    using Http::Session;

    auto entry = make_shared<TimeEntry>(request);
    auto data = nlohmann::json::object();

    data["Username"] = String::capitalize(Session(request).userName());
    string start_time;
    string note;
    auto enable_start_button
        = enableStartButton(Session(request).userId(), start_time, note);
    data["enable_start_button"] = enable_start_button;
    data["note"] = string{};
    if (!enable_start_button) {
        data["start_time"] = start_time;
        data["note"] = String::escape(note);
    }
    using DateTime::Date;

    const auto current = Date::currentDate();
    data["current_date"] = current.formatAsDate();
    data["current_date_pretty"]
        = current.formatAsWeekday() + ", " + current.formatAsDayMonth();
    data["currentLocalTime"] = String::localTime();

    auto form = std::make_shared<Input::Form>(
                    vector<Input::ElementPtr>{
                        std::make_shared<Input::Text>("event_time"),
                        std::make_shared<Input::Text>("event_type"),
                        std::make_shared<Input::Text>("note", note),
                    },
                    impl_->prefix + "/",
                    "post")
                    ->appendElement(make_shared<Input::Submit>("Create"))
                    .shared_from_this();

    return content(
               BaseTemplate(TEMPLATE_DIR "/timeentry/new.html").render(data))
        ->form(form)
        .title("Time Tracking")
        .shared_from_this();
}
std::shared_ptr<Http::Response> TimeEntryController::createEntry(
    const Request& request)
{
    if (!isAllowed(request)) {
        return content("Access denied");
    }
    using Http::Session;
    const auto prefix = impl_->prefix;
    auto entry = std::make_shared<TimeEntry>(request);
    try {
        const auto user_id = Session(request).userId();
        if (!request.hasParameter("event_time")) {
            return redirect(prefix + "/")
                ->alert("Please enter a time", Html::AlertType::DANGER)
                .shared_from_this();
        }
        if (!request.hasParameter("event_type")) {
            return redirect(prefix + "/")
                ->alert("Please enter an event type", Html::AlertType::DANGER)
                .shared_from_this();
        }
        entry->set("employee_id", user_id);
        const auto event_date
            = String::currentDate(); // TODO: provide as hidden parameter
        entry->set("event_date", event_date);
        auto event_time = request.parameter("event_time");
        entry->set("event_time", event_time);
        entry->set("event_type", request.parameter("event_type"));
        entry->set("corrected_event_id", "");
        entry->set("deleted_event_id", "");
        entry->set("creation_date", String::localDateTime());
        entry->set("creator_user_id", user_id);
        entry->set("note", request.parameter("note"));
        /* Check:
         * Can't have an event that has exactly the same date and time:
         * UNIQUE(employee_id, event_date, event_time)
         */
        if (entry->checkTimestampExists(user_id, event_date, event_time)) {
            return redirect(prefix + "/")
                ->alert(
                    "Fehler: Zeitpunkt wurde bereits erfasst",
                    Html::AlertType::DANGER)
                .shared_from_this();
        }
        /*
         * Check:
         * That the stop event cannot be before the last start event!
         * SELECT MAX(event_time) AS max_time FROM time_entry WHERE employee_id
         * = ? AND event_date = ? AND event_type = 'start';
         *
         */
        {
            using DateTime::Time;
            auto result = entry->isOpen(user_id, event_date);
            if (result.isOpen) {
                auto difference
                    = Time::parseTime(event_time)
                          .difference(Time::parseTime(result.start_time));

                if (difference.toMinutes() < 0) {
                    return redirect(prefix + "/")
                        ->alert(
                            "Fehler: Stop kann nicht vor Start sein",
                            Html::AlertType::DANGER)
                        .shared_from_this();
                }
            }
        }
        entry->insert();
    } catch (const Data::ValidationError& ex) {
        return redirect(prefix + "/")
            ->alert(ex.what(), Html::AlertType::DANGER)
            .shared_from_this();
    }
    return redirect(prefix + "/")
        ->alert(entry->presentableName() + " created", Html::AlertType::SUCCESS)
        .shared_from_this();
}
TimeEntryController& TimeEntryController::initialize(Http::Router& router)
{
    const auto prefix = impl_->prefix;
    auto ptr = shared_from_this();

    router.get(prefix + "/", [ptr](const Request& request) {
        return ptr->entryForm(request);
    });
    router.post(prefix + "/", [ptr](const Request& request) {
        return ptr->createEntry(request);
    });
#ifdef DEBUG_BUILD
    router.get(
        prefix + "/generateTestData", [ptr, prefix](const Request& request) {
            ptr->generateTwoYearsOfTestData(Http::Session(request).userId());
            return redirect(prefix + "/")
                ->alert("Test data generated", Html::AlertType::SUCCESS)
                .shared_from_this();
        });
#endif
    return *this;
}
TimeEntryController::TimeEntryController(const string& prefix)
    : impl_(new TimeEntryControllerImpl)
{
    impl_->prefix = prefix;
}
bool TimeEntryController::enableStartButton(
    const string& userId, string& start_time, string& note)
{
    auto todays_date = String::currentDate();
    auto result = TimeEntry().isOpen(userId, todays_date);
    start_time = result.start_time;
    note = result.note;
    return !result.isOpen;
}
static void insert_for_day(
    TimeEntry& t,
    const std::string& day,
    const std::string& start,
    const std::string& stop)
{
    t.set("event_date", day);
    t.set("event_time", start);
    t.set("event_type", "start");
    t.insert();
    t.set("event_date", day);
    t.set("event_time", stop);
    t.set("event_type", "stop");
    t.insert();
}
void TimeEntryController::generateTwoYearsOfTestData(const string& userId)
{
    for (std::string year : {"2022", "2023"}) {
        for (auto month :
             {"01",
              "02",
              "03",
              "04",
              "05",
              "06",
              "07",
              "08",
              "09",
              "10",
              "11",
              "12"}) {
            for (auto day :
                 {"01", "02", "03", "04", "05", "06", "07", "08", "09",
                  "10", "11", "12", "13", "14", "15", "16", "17", "18",
                  "19", "20", "21", "22", "23", "24", "25", "26"}) {
                auto date = year + "-" + month + "-" + day;
                auto start = "08:00";
                auto stop = "16:00";
                TimeEntry t;
                t.set("employee_id", userId);
                insert_for_day(t, date, start, stop);
            }
        }
    }
}
bool TimeEntryController::isAllowed(const Request& request)
{
    using Http::Session;
    Session session(request);
    return (session.role() == "user") || session.isAdmin();
}
TimeEntryController::~TimeEntryController() = default;
