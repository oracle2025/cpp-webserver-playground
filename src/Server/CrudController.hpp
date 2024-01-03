#pragma once

#include "Confirm.hpp"
#include "Data/Todo.hpp"
#include "Form.hpp"
#include "Http/NotFoundHandler.hpp"
#include "Http/NullHandler.hpp"
#include "Http/Request.hpp"
#include "Http/Response.hpp"
#include "Http/Session.hpp"
#include "List.hpp"
#include "Server/WebServer.hpp"
#include "Submit.hpp"
#include "bunfet-example.hpp"
#include "style.hpp"
/*
 * A Simple Todo List:
 * Data Model:
 *
 * id (uuid)
 * Description (text)
 * Checked (bool)
 * created_at (date)
 * updated_at (date)
 *
 * CREATE TABLE
 * INSERT
 * UPDATE
 * DELETE FROM
 *
 * ActiveRecord {
 *   uuid id;
 *   string description;
 *   bool checked;
 *   std::chrono::time_point<std::chrono::system_clock> created_at;
 *   std::chrono::time_point<std::chrono::system_clock> updated_at;
 *   update();
 *   insert();
 *   delete();
 * }
 *
 * https://stackoverflow.com/questions/23038996/stdchronohigh-resolution-clocktime-point-cast-to-store-in-sqlite
 *     using namespace std::literals; // enables the usage of 24h, 1ms, 1s
 instead of
                                   // e.g. std::chrono::hours(24), accordingly
    const std::chrono::time_point<std::chrono::system_clock> now =
        std::chrono::system_clock::now();
 */

using Http::content;
using Http::redirect;

template<typename T, typename F>
struct CrudController : public T {
    using Response = Http::Response;
    using Request = Http::Request;
    CrudController(const string& prefix)
    {
        static_assert(
            std::is_base_of<Record, F>::value, "F not derived from Record");
        static_assert(
            std::is_base_of<WebServer, T>::value,
            "T not derived from WebServer");
        T::router().get(prefix + "/new", [this, prefix](const Request& request) {
            using namespace Input;
            F record = makeRecord(request);
            return content(Form(record, prefix + "/create", "post")
                               .appendElement(make_shared<Submit>(
                                   "Create " + record.presentableName()))())
                ->appendNavBarAction({"Start", "/"})
                .title("Create " + record.presentableName())
                .shared_from_this();
        });
        T::router().post(prefix + "/create", [this, prefix](const Request& request) {
            using Http::Session;
            F record = makeRecord(request);
            for (auto field : record.fields()) {
                if (request.hasParameter(field)) {
                    record.set(field, request.parameter(field));
                }
                if (field == "user_id") {
                    record.set("user_id", Session(request).userId());
                }
            }
            record.insert();
            return redirect(prefix + "/edit?" + record.key())
                ->alert(
                    record.presentableName() + " created",
                    Html::AlertType::SUCCESS)
                .shared_from_this();
        });
        T::router().get(prefix + "/edit", [this, prefix](const Request& request) {
            using namespace Input;
            F record = makeRecord(request);
            if (record.pop(request.query())) {
                return content(Form(
                                   record,
                                   string(prefix + "/update?") + record.key(),
                                   "post")
                                   .appendElement(make_shared<Submit>(
                                       "Update " + record.presentableName()))())
                    ->appendNavBarAction({"Start", "/"})
                    .title("Edit " + record.presentableName())
                    .shared_from_this();
            } else {
                return recordNotFound(prefix, record.presentableName());
            }
        });
        T::router().post(prefix + "/update", [this, prefix](const Request& request) {
            F record = makeRecord(request);
            if (record.pop(request.query())) {
                for (auto i : record.fields()) {
                    if (request.hasParameter(i)) {
                        record.set(i, request.parameter(i));
                    }
                }
                record.update();
                return redirect(prefix + "/edit?" + record.key())
                    ->alert(
                        record.presentableName() + " updated",
                        Html::AlertType::SUCCESS)
                    .shared_from_this();
            } else {
                return recordNotFound(prefix, record.presentableName());
            }
        });
        T::router().post(prefix + "/mark", [this, prefix](const Request& request) {
            F record = makeRecord(request);
            std::ostringstream str;
            for (const auto& [key, value] : request.allParameters()) {
                record.pop(key);
                if (record.get("checked") != value) {
                    str << record.get("description") << " is now "
                        << (value == "yes" ? "checked" : "unchecked")
                        << std::endl;
                }
                record.set("checked", value);
                record.update();
            }
            return redirect(prefix + "/")
                ->alert("Todo " + str.str(), Html::AlertType::SUCCESS)
                .shared_from_this();
        });
        T::router().post(prefix + "/delete", [this, prefix](const Request& request) {
            F record = makeRecord(request);
            if (record.pop(request.query())) {
                if (request.hasParameter("confirmed")) {
                    record.erase();
                    return redirect(prefix + "/")
                        ->alert("Todo deleted", Html::AlertType::WARNING)
                        .shared_from_this();
                } else if (request.hasParameter("canceled")) {
                    return redirect(prefix + "/")
                        ->alert("Delete canceled", Html::AlertType::INFO)
                        .shared_from_this();
                } else {
                    return redirect(prefix + "/confirm?" + record.key())
                        ->alert(
                            "Are you sure you want to delete this todo?",
                            Html::AlertType::WARNING)
                        .shared_from_this();
                }
            } else {
                return recordNotFound(prefix, record.presentableName());
            }
        });
        T::router().get(prefix + "/delete", [this, prefix](const Request& request) {
            F record = makeRecord(request);
            if (record.pop(request.query())) {
                return redirect(prefix + "/confirm?" + record.key())
                    ->alert(
                        "Are you sure you want to delete this "
                        "todo?",
                        Html::AlertType::WARNING)
                    .shared_from_this();
            } else {
                return recordNotFound(prefix, record.presentableName());
            }
        });
        T::router().get(prefix + "/confirm", [this, prefix](const Request& request) {
            using namespace Input;
            F record = makeRecord(request);
            if (record.pop(request.query())) {
                return Confirm(prefix, record, record.description())()
                    ->appendNavBarAction({"Start", "/"})
                    .shared_from_this();
            } else {
                return recordNotFound(prefix, record.presentableName());
            }
        });
        T::router().get(prefix + "/", [this, prefix](const Request& request) {
            F record = makeRecord(request);
            using namespace Input;
            auto columns = record.presentableFields();
            return content(Form(
                               Html::List(record.listAsPointers(), columns)
                                   .prefix(prefix)(),
                               prefix + "/mark",
                               "post")())
                ->appendAction(
                    {"Create new " + record.presentableName(), prefix + "/new"})
                .appendNavBarAction({"Start", "/"})
                .title(record.presentableName() + " List")
                .shared_from_this();
        });
        if (!prefix.empty()) {
            T::router().get("/", [prefix](const Request& request) {
                return redirect(prefix + "/");
            });
        }
        T::router().get("/css/style.css", [](const Request& request) {
            return content(STYLE_SHEET, "text/css");
        });
        // Return bootstrap3 sample for testing in HaikuOS
        T::router().get("/bunfet-example/", [](const Request& request) {
            return content(BUNFET_EXAMPLE)
                ->noPresentation(true)
                .shared_from_this();
        });
        T::defaultHandler(Http::NullHandler);
        T::finish_init();
    }
    static shared_ptr<Response> recordNotFound(
        const string& prefix, const string& presentableName)
    {
        return content(presentableName + " not found")
            ->code(Response::NOT_FOUND)
            .appendNavBarAction({"Start", "/"})
            .shared_from_this();
    }
    virtual F makeRecord(const Request& request)
    {
        F record(request);
        return record;
    }
};