#pragma once

#include <memory>
#include <vector>

namespace Http {

class RequestHandler;

using RequestHandlerList = std::vector<std::shared_ptr<RequestHandler>>;

} // namespace Http