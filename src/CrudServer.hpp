#pragma once

#include "Request.hpp"
#include "Todo.hpp"
#include "Form.hpp"
#include "Response.hpp"
#include "List.hpp"

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
 *     using namespace std::literals; // enables the usage of 24h, 1ms, 1s instead of
                                   // e.g. std::chrono::hours(24), accordingly
    const std::chrono::time_point<std::chrono::system_clock> now =
        std::chrono::system_clock::now();
 */

template<typename T>
struct CrudServer : public T {
    CrudServer() {
        T::get("/new", [](const Request &request) {
            using namespace Input;
            Todo todo;
            return content(R"(<a href="/list">list</a><br>)" +
                           Form(todo, "/create", "post")());
        });
        T::post("/create", [this](const Request &request) {
            Todo todo;
            for (auto i: todo.fields()) {
                if (request.hasParameter(i)) {
                    todo.set(i, request.parameter(i));
                }
            }
            todo.insert();
            return content(string{"Todo created<br>"} + R"(<a href="/list">list</a><br>)" +
                           Input::Form(todo, "/update", "post")());
            //return redirect_to("/read/" + m_id.toString());
        });
        T::get("/edit", [](const Request &request) {
            Todo todo;
            todo.pop(request.query());
            return content(R"(<a href="/list">list</a><br>)" +
                           Input::Form(todo, "/update", "post")());
        });
        T::post("/update", [](const Request &request) {
            Todo todo;
            todo.pop(request.query());
            for (auto i: todo.fields()) {
                if (request.hasParameter(i)) {
                    todo.set(i, request.parameter(i));
                }
            }
            todo.update();
            return content(string{"Todo updated<br>"} + R"(<a href="/list">list</a><br>)" +
                           Input::Form(todo, "/edit-submit", "post")());
        });
        T::get("/delete", [](const Request &request) {
            return content("");
        });
        T::get("/list", [](const Request &request) {
            return content(R"(<a href="/new">Create new todo</a><br>)" +
                           Html::List(Todo::listAsPointers())());
        });
        T::finish_init();
    }
};