#pragma once

#include <functional>
#include <map>
#include <memory>
#include <string>

namespace Http {

using std::function;
using std::map;
using std::shared_ptr;
using std::string;

struct Request;
struct Response;

using handler_type = function<shared_ptr<Response>(const Request& request)>;

class Router : public map<string, handler_type> {
public:
    handler_type &findHandlerOrReturnDefault(const string& path, handler_type& defaultHandler);

    Router& get(const string& path, handler_type handler);

};

} // namespace Http