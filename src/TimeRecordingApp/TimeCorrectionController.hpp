#pragma once

#include <nlohmann/json.hpp>

#include <iosfwd>
#include <memory>

namespace Http {
class Request;
class Response;
class Router;
} // namespace Http
namespace DateTime {
class Time;
} // namespace DateTime
class Record;

class TimeCorrectionController
    : public std::enable_shared_from_this<TimeCorrectionController> {
public:
    using string = std::string;
    using Response = Http::Response;
    using Request = Http::Request;
    ~TimeCorrectionController();

    explicit TimeCorrectionController(const string& prefix);
    TimeCorrectionController& initialize(Http::Router& router);

    static nlohmann::json convertResultToData(
        const std::vector<std::shared_ptr<Record>>& records,
        DateTime::Time& total);

private:
    std::shared_ptr<Response> listEntries(const Request& request);
    std::shared_ptr<Response> editEntry(const Request& request);
    std::shared_ptr<Response> updateEntry(const Request& request);
    std::shared_ptr<Response> newEntry(const Request& request);
    std::shared_ptr<Response> createEntry(const Request& request);


    int selectYear(const Request& request, const std::vector<int>& years);
    int selectMonth(const Request& request, const std::vector<int>& months);

    struct TimeCorrectionControllerImpl;
    std::unique_ptr<TimeCorrectionControllerImpl> impl_;
    bool isAllowed(const Request& request);

};
