#pragma once

#include "Form.hpp"
#include "List.hpp"
#include "Request.hpp"
#include "Response.hpp"
#include "Todo.hpp"
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
            return content(
                R"(<a href="/list">list</a><br>)"
                + Form(todo, "/create", "post")());
        });
        T::post("/create", [this](const Request& request) {
            Todo todo;
            for (auto i : todo.fields()) {
                if (request.hasParameter(i)) {
                    todo.set(i, request.parameter(i));
                }
            }
            todo.insert();
            return content(
                string{"Todo created<br>"} + R"(<a href="/list">list</a><br>)"
                + Input::Form(todo, "/update", "post")());
            // return redirect_to("/read/" + m_id.toString());
        });
        T::get("/edit", [](const Request& request) {
            Todo todo;
            if (todo.pop(request.query())) {
                return content(
                    R"(<a href="/list">list</a><br>)"
                    + Input::Form(todo, "/update", "post")());
            } else {
                return content("Todo not found")->code(Response::NOT_FOUND).shared_from_this();
            }
        });
        T::post("/update", [](const Request& request) {
            Todo todo;
            todo.pop(request.query());
            for (auto i : todo.fields()) {
                if (request.hasParameter(i)) {
                    todo.set(i, request.parameter(i));
                }
            }
            todo.update();
            return content(
                string{"Todo updated<br>"} + R"(<a href="/list">list</a><br>)"
                + Input::Form(todo, "/edit-submit", "post")());
        });
        T::get("/delete", [](const Request& request) { return content(""); });
        T::get("/list", [](const Request& request) {
            const string header = R"(<!doctype html><html lang="de"><head>
<meta charset="utf-8">
<meta name="viewport" content="width=device-width, initial-scale=1.0">
<link rel="stylesheet" type="text/css" href="css/style.css">
</head>
<body>
<div class="container">
)";
            return content(
                header +
                R"(<a href="/new">Create new todo</a><br>)"
                + Html::List(Todo::listAsPointers(), {"checked", "description"})()
                + R"(</div></body></html>)");
        });
        T::get("/css/style.css", [](const Request& request) {
            return content(STYLE_SHEET,
                           "text/css");
        });
        T::finish_init();
    }
};