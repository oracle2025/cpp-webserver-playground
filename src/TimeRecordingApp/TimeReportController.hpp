#pragma once

#include <iosfwd>
#include <memory>

namespace Http {
class Request;
class Response;
class Router;
} // namespace Http

class TimeReportController
    : public std::enable_shared_from_this<TimeReportController> {
public:
    using Response = Http::Response;
    using Request = Http::Request;
    ~TimeReportController();

    explicit TimeReportController(const std::string& prefix);
    TimeReportController& initialize(Http::Router& router);

private:
    std::shared_ptr<Response> list(const Request& request);

    int selectYear(const Request& request, const std::vector<int>& years);
    int selectMonth(const Request& request, const std::vector<int>& months);

    struct TimeReportControllerImpl;
    std::unique_ptr<TimeReportControllerImpl> impl_;
};
