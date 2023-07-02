#pragma once

#include "Data/User.hpp"
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
    using Response = Http::Response;
    using Session = Http::Session;
    SignupController(const string& prefix)
    {
        using namespace Input;
        T::router().get(prefix + "/", [](const Request& request) {
            return content(Form(
                               {Text("username")(),
                                Password("password")(),
                                Password("confirm_password")(),
                                Submit("Signup")()},
                               "/signup",
                               "post")())
                ->title("Signup")
                .shared_from_this();
        });
        T::router().get(prefix + "/submit", [prefix](const Request& request) {
            User user;
            if (findUser(*g_session, request.parameter("username"), user)) {
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
            user.username = request.parameter("username");
            user.password = request.parameter("password");
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