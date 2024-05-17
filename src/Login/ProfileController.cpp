

#include "ProfileController.hpp"

#include "Data/User.hpp"
#include "Http/Session.hpp"
#include "Request.hpp"
#include "Response.hpp"
#include "Router.hpp"
#include "String/escape.hpp"
#include "Template/BaseTemplate.hpp"

namespace Login {

using Http::content;

ProfileController& ProfileController::initialize(Http::Router& router)
{
    const std::string prefix = "/profile";
    using Data::User;
    using Http::Request;
    using Http::Response;
    using nlohmann::json;
    using Template::BaseTemplate;
    router.get(prefix + "/", [](const Request& request) {
        auto user = std::make_shared<User>();
        if (user->pop(Http::Session(request).userId())) {
            auto data = json::object();
            data["start_page"] = String::escape(user->values()["start_page"]);
            return content(BaseTemplate(TEMPLATE_DIR "/profile/edit.html")
                               .render(data))
                ->title(
                    "Edit Profile: "
                    + String::escape(user->values()["username"]))
                .shared_from_this();
        } else {
            throw std::runtime_error("Logged in User not found");
        }
    });
    router.post(prefix + "/update", [prefix](const Request& request) {
        auto user = std::make_shared<User>();
        using Http::redirect;
        if (user->pop(Http::Session(request).userId())) {
            for (const auto& param_name : {"start_page"}) {
                if (request.hasParameter(param_name)) {
                    user->setImpl(param_name, request.parameter(param_name));
                }
            }
            user->update();
            return redirect(prefix + "/")
                ->alert("Profile updated", Html::AlertType::SUCCESS)
                .shared_from_this();
        } else {
            throw std::runtime_error("Logged in User not found");
        }
    });

    return *this;
}
} // namespace Login