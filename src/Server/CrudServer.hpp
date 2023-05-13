#pragma once

#include "Data/Todo.hpp"
#include "Form.hpp"
#include "Http/Request.hpp"
#include "Http/Response.hpp"
#include "Http/NotFoundHandler.hpp"
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

using namespace Http;

template<typename T>
struct CrudServer : public T {
    CrudServer()
    {
        T::router().get("/new", [](const Request& request) {
            using namespace Input;
            Todo todo;
            return content(Form(todo, "/create", "post")
                               .appendElement(Submit("Create Todo")())())
                ->appendNavBarAction({"Start", "/"})
                .title("Create Todo")
                .shared_from_this();
        });
        T::router().get("/create", [this](const Request& request) {
            Todo todo;
            for (auto i : todo.fields()) {
                if (request.hasParameter(i)) {
                    todo.set(i, request.parameter(i));
                }
            }
            todo.insert();
            return redirect("/edit?" + todo.key())
                ->alert("Todo created", Html::AlertType::SUCCESS)
                .shared_from_this();
        });
        T::router().get("/edit", [](const Request& request) {
            using namespace Input;
            Todo todo;
            if (todo.pop(request.query())) {
                return content(
                           Form(todo, string("/update?") + todo.key(), "post")
                               .appendElement(Submit("Update Todo")())())
                    ->appendNavBarAction({"Start", "/"})
                    .title("Edit Todo")
                    .shared_from_this();
            } else {
                return todoNotFound();
            }
        });
        T::router().get("/update", [](const Request& request) {
            Todo todo;
            if (todo.pop(request.query())) {
                for (auto i : todo.fields()) {
                    if (request.hasParameter(i)) {
                        todo.set(i, request.parameter(i));
                    }
                }
                todo.update();
                return redirect("/edit?" + todo.key())
                    ->alert("Todo updated", Html::AlertType::SUCCESS)
                    .shared_from_this();
            } else {
                return todoNotFound();
            }
        });
        T::router().get("/delete", [](const Request& request) {
            Todo todo;
            if (todo.pop(request.query())) {
                todo.erase();
                return redirect("/")
                    ->alert("Todo deleted", Html::AlertType::WARNING)
                    .shared_from_this();
            } else {
                return todoNotFound();
            }
        });
        T::router().get("/", [](const Request& request) {
            return content(Html::List(
                               Todo::listAsPointers(),
                               {"checked", "description"})())
                ->appendAction({"Create new Todo", "/new"})
                .appendNavBarAction({"Start", "/"})
                .title("Todo List")
                .shared_from_this();
        });
        T::router().get("/css/style.css", [](const Request& request) {
            return content(STYLE_SHEET, "text/css");
        });
        T::defaultHandler(Http::NotFoundHandler);
        T::finish_init();
    }
    static shared_ptr<Response> todoNotFound()
    {
        return content("Todo not found")
            ->code(Response::NOT_FOUND)
            .appendNavBarAction({"Start", "/"})
            .shared_from_this();
    }
};