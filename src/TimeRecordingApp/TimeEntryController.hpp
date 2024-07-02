#pragma once

#include <iosfwd>
#include <memory>

namespace Http {
class Request;
class Response;
class Router;
} // namespace Http

struct TimeEntryController
    : public std::enable_shared_from_this<TimeEntryController> {
    using string = std::string;
    using Response = Http::Response;
    using Request = Http::Request;
    ~TimeEntryController();

    explicit TimeEntryController(const string& prefix);
    TimeEntryController& initialize(Http::Router& router);

private:
    std::shared_ptr<Response> createEntry(const Request& request);
    std::shared_ptr<Response> entryForm(const Request& request);
    struct TimeEntryControllerImpl;
    std::unique_ptr<TimeEntryControllerImpl> impl_;
};
