#pragma once

#include <iosfwd>
#include <memory>

namespace Http {
class Request;
class Response;
class Router;
} // namespace Http

class TimeCorrectionController
    : public std::enable_shared_from_this<TimeCorrectionController> {
public:
    using string = std::string;
    using Response = Http::Response;
    using Request = Http::Request;
    ~TimeCorrectionController();

    explicit TimeCorrectionController(const string& prefix);
    TimeCorrectionController& initialize(Http::Router& router);

private:
    std::shared_ptr<Response> listEntries(const Request& request);
    std::shared_ptr<Response> editEntry(const Request& request);
    std::shared_ptr<Response> updateEntry(const Request& request);

    struct TimeCorrectionControllerImpl;
    std::unique_ptr<TimeCorrectionControllerImpl> impl_;
};
