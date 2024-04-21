#pragma once

#include <functional>
#include <iosfwd>
#include <memory>

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

namespace Http {
class Request;
class Response;
class Router;
} // namespace Http
struct RecordExtended;
using std::shared_ptr;

struct CrudController : public std::enable_shared_from_this<CrudController>{
    using string = std::string;
    using Response = Http::Response;
    using Request = Http::Request;
    using make_record_func = std::function<std::shared_ptr<RecordExtended>(
        const Request& request)>;
    CrudController(
        const string& prefix,
        make_record_func makeRecordFunc);
    virtual CrudController& initialize(Http::Router& router);
    virtual ~CrudController();
    static shared_ptr<Response> recordNotFound(
        const string& prefix, const string& presentableName);
    const string& prefix() const;

protected:
    virtual std::shared_ptr<Response> editRecord(const Request& request);
    virtual std::shared_ptr<Response> listRecords(const Request& request);

    make_record_func m_makeRecord;

private:
    struct CrudControllerImpl;
    std::unique_ptr<CrudControllerImpl> impl_;
};