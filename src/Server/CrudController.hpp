#pragma once

#include "Confirm.hpp"
#include "Data/Todo.hpp"
#include "Form.hpp"
#include "Http/NotFoundHandler.hpp"
#include "Http/NullHandler.hpp"
#include "Http/Request.hpp"
#include "Http/Session.hpp"
#include "Http/Response.hpp"
#include "List.hpp"
#include "Submit.hpp"
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
        T::router().get(prefix + "/new", [prefix](const Request& request) {
            using namespace Input;
            F todo;
            return content(Form(todo, prefix + "/create", "post")
                               .appendElement(Submit(
                                   "Create " + F::presentableName())())())
                ->appendNavBarAction({"Start", prefix + "/"})
                .title("Create " + F::presentableName())
                .shared_from_this();
        });
        T::router().get(prefix + "/create", [prefix](const Request& request) {
            using Http::Session;
            F todo(request);
            for (auto i : todo.fields()) {
                if (request.hasParameter(i)) {
                    todo.set(i, request.parameter(i));
                }
                todo.set("user_id", Session(request).userId());
            }
            todo.insert();
            return redirect(prefix + "/edit?" + todo.key())
                ->alert("Todo created", Html::AlertType::SUCCESS)
                .shared_from_this();
        });
        T::router().get(prefix + "/edit", [prefix](const Request& request) {
            using namespace Input;
            F todo;
            if (todo.pop(request.query())) {
                return content(Form(
                                   todo,
                                   string(prefix + "/update?") + todo.key(),
                                   "post")
                                   .appendElement(Submit(
                                       "Update " + F::presentableName())())())
                    ->appendNavBarAction({"Start", prefix + "/"})
                    .title("Edit " + F::presentableName())
                    .shared_from_this();
            } else {
                return todoNotFound(prefix);
            }
        });
        T::router().get(prefix + "/update", [prefix](const Request& request) {
            F todo;
            if (todo.pop(request.query())) {
                for (auto i : todo.fields()) {
                    if (request.hasParameter(i)) {
                        todo.set(i, request.parameter(i));
                    }
                }
                todo.update();
                return redirect(prefix + "/edit?" + todo.key())
                    ->alert(
                        F::presentableName() + " updated",
                        Html::AlertType::SUCCESS)
                    .shared_from_this();
            } else {
                return todoNotFound(prefix);
            }
        });
        T::router().get(prefix + "/mark", [prefix](const Request& request) {
            F todo;
            std::ostringstream str;
            for (const auto& [key, value] : request.allParameters()) {
                todo.pop(key);
                if (todo.get("checked") != value) {
                    str << todo.get("description") << " is now "
                        << (value == "yes" ? "checked" : "unchecked")
                        << std::endl;
                }
                todo.set("checked", value);
                todo.update();
            }
            return redirect(prefix + "/")
                ->alert("Todo " + str.str(), Html::AlertType::SUCCESS)
                .shared_from_this();
        });
        T::router().get(prefix + "/delete", [prefix](const Request& request) {
            F todo;
            if (todo.pop(request.query())) {
                if (request.hasParameter("confirmed")) {
                    todo.erase();
                    return redirect(prefix + "/")
                        ->alert("Todo deleted", Html::AlertType::WARNING)
                        .shared_from_this();
                } else if (request.hasParameter("canceled")) {
                    return redirect(prefix + "/")
                        ->alert("Delete canceled", Html::AlertType::INFO)
                        .shared_from_this();
                } else {
                    return redirect(prefix + "/confirm?" + todo.key())
                        ->alert(
                            "Are you sure you want to delete this todo?",
                            Html::AlertType::WARNING)
                        .shared_from_this();
                }
            } else {
                return todoNotFound(prefix);
            }
        });
        T::router().get(prefix + "/confirm", [prefix](const Request& request) {
            using namespace Input;
            F todo;
            if (todo.pop(request.query())) {
                return Confirm(prefix, todo, todo.description())()
                    ->appendNavBarAction({"Start", prefix + "/"})
                    .shared_from_this();
            } else {
                return todoNotFound(prefix);
            }
        });
        T::router().get(prefix + "/", [prefix](const Request& request) {
            F todo(request);
            using namespace Input;
            auto columns = todo.presentableFields();
            return content(Form(
                               {Html::List(todo.listAsPointers(), columns)
                                    .prefix(prefix)()},
                               prefix + "/mark",
                               "post")())
                ->appendAction(
                    {"Create new " + F::presentableName(), prefix + "/new"})
                .appendNavBarAction({"Start", prefix + "/"})
                .title(F::presentableName() + " List")
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
        T::defaultHandler(Http::NullHandler);
        T::finish_init();
    }
    static shared_ptr<Response> todoNotFound(const string& prefix)
    {
        return content(F::presentableName() + " not found")
            ->code(Response::NOT_FOUND)
            .appendNavBarAction({"Start", prefix + "/"})
            .shared_from_this();
    }
};