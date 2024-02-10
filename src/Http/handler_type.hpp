#pragma once
#include <functional>
#include <memory>

namespace Http {
class Request;
class Response;
using handler_type = std::function<std::shared_ptr<Response>(const Request& request)>;
} // namespace Http