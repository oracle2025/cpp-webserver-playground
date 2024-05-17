#pragma once

namespace Http {
class Router;
}

namespace Document {

struct DocumentController {
    static void initialize(Http::Router& router);
};

} // namespace Document
