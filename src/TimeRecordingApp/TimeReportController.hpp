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

    struct TimeReportControllerImpl;
    std::unique_ptr<TimeReportControllerImpl> impl_;
};
