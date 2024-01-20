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
#include "Router.hpp"
#include "Server/WebServer.hpp"
#include "Submit.hpp"
#include "bunfet-example.hpp"
#include "style.hpp"

#include <utility>
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

struct CrudController {
    using Response = Http::Response;
    using Request = Http::Request;
    using make_record_func = std::function<std::shared_ptr<RecordExtended>(
        const Request& request)>;
    CrudController(
        const string& prefix,
        make_record_func makeRecordFunc,
        Http::Router& router);
    virtual ~CrudController() = default;
    static shared_ptr<Response> recordNotFound(
        const string& prefix, const string& presentableName);

    make_record_func m_makeRecord;
};