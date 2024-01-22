#pragma once

#include "Data/User.hpp"
#include "Http/NullHandler.hpp"
#include "Http/Request.hpp"
#include "Http/Response.hpp"
#include "Http/Session.hpp"
#include "Input/Form.hpp"
#include "Input/Password.hpp"
#include "Input/Submit.hpp"
#include "Input/Text.hpp"

namespace Signup {

using Http::content;
using Http::redirect;

template<typename T>
struct SignupController : public T {
    using Request = Http::Request;
    using Session = Http::Session;
    using Response = Http::Response;
    explicit SignupController(const string& prefix)
    {
        using namespace Input;
        T::router().get(prefix + "/", [prefix](const Request& request) {
            auto form = make_shared<Form>(
                vector<ElementPtr>{make_shared<Text>("username"),
                 make_shared<Password>("password"),
                 make_shared<Password>("confirm_password"),
                 make_shared<Submit>("Signup")},
                prefix + "/submit",
                "post");
            return content((*form)())
                ->title("Signup")
                .form(form)
                .shared_from_this();
        });
        T::router().post(prefix + "/submit", [prefix](const Request& request) {
            Data::User user;
            const auto username = request.parameter("username");
            if (username == "admin" || findUser(*g_session, username, user)) {
                return redirect(prefix + "/")
                    ->alert("User already exists", Html::AlertType::INFO)
                    .shared_from_this();
            }
            if (request.parameter("password")
                != request.parameter("confirm_password")) {
                return redirect(prefix + "/")
                    ->alert("Passwords do not match", Html::AlertType::WARNING)
                    .shared_from_this();
            }
            user.username = username;
            user.setPassword(request.parameter("password"));
            user.insert();

            auto response
                = redirect("/")
                      ->alert("User created", Html::AlertType::SUCCESS)
                      .shared_from_this();
            Session(request).clearSession();
            Session(request).current(*response).login(user);
            return response;
        });
        T::defaultHandler(Http::NullHandler);
        T::finish_init();
    }
};

} // namespace Signup