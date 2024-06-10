#include "Document/DocumentController.hpp"

#include "Http/Response.hpp"
#include "Http/Router.hpp"
#include "Template/BaseTemplate.hpp"

namespace Document {

using Http::content;
using Template::BaseTemplate;

void DocumentController::initialize(Http::Router& router)
{
    const std::string prefix = "/document";
    router.get(prefix + "/", [](const Http::Request& request) {
        return content(BaseTemplate(TEMPLATE_DIR "/document/list.html")());
    });
    router.get(prefix + "/new", [](const Http::Request& request) {
        return content(BaseTemplate(TEMPLATE_DIR "/document/new.html")());
    });
    router.get(prefix + "/view", [](const Http::Request& request) {
        // Add some tag to the content that this template will provide its own
        // container
        auto data = nlohmann::json::object();
        data["title"] = "";
        data["alerts"] = "";
        data["navBarActions"] = "";
        data["actions"] = "";
        data["content"] = BaseTemplate(TEMPLATE_DIR "/document/view.html")();
        return content(
                   BaseTemplate(TEMPLATE_DIR "/index_wide.html").render(data))
            ->noPresentation(true)
            .shared_from_this();
    });
    router.get(
        prefix + "/generic-placeholder-page.pdf",
        [](const Http::Request& request) {
            return std::make_shared<Http::Response>()
                ->sendFile("../downloads/generic-placeholder-page.pdf")
                .mimetype("application/pdf")
                .shared_from_this();
        });
}

} // namespace Document