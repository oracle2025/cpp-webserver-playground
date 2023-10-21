#pragma once

#include "Data/User.hpp"
#include "Http/NotFoundHandler.hpp"
#include "Http/Request.hpp"
#include "Http/Response.hpp"
#include "Http/Session.hpp"
#include "Input/Form.hpp"
#include "Input/Password.hpp"
#include "Input/Submit.hpp"

#include <string>

using Http::content;
using std::string;

template<typename T>
struct PasswordChangeController : public T {
    using Request = Http::Request;
    using Response = Http::Response;
    using Session = Http::Session;
    PasswordChangeController(const string& prefix)
    {
        T::router().get(prefix + "/", [prefix](const Request& request) {
            using namespace Input;
            Data::User user;
            if (user.pop(Session(request).userId())) { // current session user
                return content(
                           Form(
                               {Password("current_password")(),
                                Password("new_password")(),
                                Password("confirm_password")()},
                               string(prefix + "/update"),
                               "post")
                               .appendElement(make_shared<Submit>("Update Password"))())
                    ->appendNavBarAction({"Start", "/"})
                    .title("Change Password")
                    .shared_from_this();
            } else {
                return content("User not found")
                    ->code(Response::NOT_FOUND)
                    .appendNavBarAction({"Start", "/"})
                    .shared_from_this();
                ;
            }
        });
        T::router().get(prefix + "/update", [prefix](const Request& request) {
            Data::User user;
            if (user.pop(Session(request).userId())) {
                if (request.parameter("new_password")
                    != request.parameter("confirm_password")) {
                    return content("Passwords do not match")
                        ->code(Response::NOT_FOUND)
                        .appendNavBarAction({"Start", "/"})
                        .shared_from_this();
                }
                if (!user.isValidUser(
                        user.username,
                        request.parameter("current_password"),
                        user)) {
                    return content("Current password is incorrect")
                        ->code(Response::NOT_FOUND)
                        .appendNavBarAction({"Start", "/"})
                        .shared_from_this();
                }
                user.setPassword(request.parameter("new_password"));
                user.update();
                return content("Password updated successfully");
            } else {
                return content("User not found")
                    ->code(Response::NOT_FOUND)
                    .appendNavBarAction({"Start", "/"})
                    .shared_from_this();
                ;
            }
        });
        if (!prefix.empty()) {
            T::router().get("/", [prefix](const Request& request) {
                return redirect(prefix + "/");
            });
        }
        T::defaultHandler(Http::NullHandler);
        T::finish_init();
    }
};