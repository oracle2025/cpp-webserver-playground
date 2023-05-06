#pragma once

#include "Data/Todo.hpp"
#include "Form.hpp"
#include "Http/Request.hpp"
#include "Http/Response.hpp"
#include "List.hpp"
#include "Submit.hpp"
#include "style.hpp"

/*
 * A Simple Todo List:
 * Data Model:
 *
 * m_id (uuid)
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
 *   uuid m_id;
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

template<typename T>
struct CrudServer : public T {
    CrudServer()
    {
        T::get("/new", [](const Request& request) {
            using namespace Input;
            Todo todo;
            return content(Form(todo, "/create", "post")
                               .appendElement(Submit("Create Todo")())())
                ->appendAction({"List", "/"})
                .title("Create Todo")
                .shared_from_this();
        });
        T::post("/create", [this](const Request& request) {
            Todo todo;
            for (auto i : todo.fields()) {
                if (request.hasParameter(i)) {
                    todo.set(i, request.parameter(i));
                }
            }
            todo.insert();
            return redirect("/edit?" + todo.id())
                ->alert("Todo created", Html::AlertType::SUCCESS)
                .shared_from_this();
        });
        T::get("/edit", [](const Request& request) {
            using namespace Input;
            Todo todo;
            if (todo.pop(request.query())) {
                return content(
                           Form(todo, string("/update?") + todo.id(), "post")
                               .appendElement(Submit("Update Todo")())())
                    ->appendAction({"List", "/"})
                    .title("Edit Todo")
                    .shared_from_this();
            } else {
                return content("Todo not found")
                    ->code(Response::NOT_FOUND)
                    .shared_from_this();
            }
        });
        T::post("/update", [](const Request& request) {
            Todo todo;
            if (todo.pop(request.query())) {
                for (auto i : todo.fields()) {
                    if (request.hasParameter(i)) {
                        todo.set(i, request.parameter(i));
                    }
                }
                todo.update();
                return redirect("/edit?" + todo.id())
                    ->alert("Todo updated", Html::AlertType::SUCCESS)
                    .shared_from_this();
            } else {
                return content("Todo not found")
                    ->code(Response::NOT_FOUND)
                    .shared_from_this();
            }
        });
        T::get("/delete", [](const Request& request) {
            Todo todo;
            if (todo.pop(request.query())) {
                todo.erase();
                return redirect("/")
                    ->alert("Todo deleted", Html::AlertType::WARNING)
                    .shared_from_this();
            } else {
                return content("Todo not found")
                    ->code(Response::NOT_FOUND)
                    .shared_from_this();
            }
        });
        T::get("/", [](const Request& request) {
            return content(Html::List(
                               Todo::listAsPointers(),
                               {"checked", "description"})())
                ->appendAction({"Create new Todo", "/new"})
                .title("Todo List")
                .shared_from_this();
        });
        T::get("/css/style.css", [](const Request& request) {
            return content(STYLE_SHEET, "text/css");
        });
        T::finish_init();
    }
};