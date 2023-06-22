#pragma once

#include "Data/User.hpp"
#include "Http/NotFoundHandler.hpp"
#include "Http/Request.hpp"
#include "Http/Response.hpp"
#include "Input/Form.hpp"
#include "Input/Password.hpp"
#include "Input/Submit.hpp"

#include <string>

using Http::content;
using std::string;

template<typename T>
struct PasswordChangeComponent : public T {
    using Request = Http::Request;
    using Response = Http::Response;
    PasswordChangeComponent(const string& prefix)
    {
        T::router().get(prefix + "/", [prefix](const Request& request) {
            using namespace Input;
            User user;
            if (user.pop(request.query())) { // current session user
                                             // request.session().get("user")
                return content(
                           Form(
                               {Password("current_password")(),
                                Password("new_password")(),
                                Password("confirm_password")()},
                               string(prefix + "/update?") + user.key(),
                               "post")
                               .appendElement(Submit("Update Password")())())
                    ->appendNavBarAction({"Start", prefix + "/"})
                    .title("Change Password")
                    .shared_from_this();
            } else {
                return content("User not found")
                    ->code(Response::NOT_FOUND)
                    .appendNavBarAction({"Start", prefix + "/"})
                    .shared_from_this();
                ;
            }
        });
        T::router().get(prefix + "/update", [prefix](const Request& request) {
            User user;
            if (user.pop(request.query())) {
                if (request.parameter("new_password")
                    != request.parameter("confirm_password")) {
                    return content("Passwords do not match")
                        ->code(Response::NOT_FOUND)
                        .appendNavBarAction({"Start", prefix + "/"})
                        .shared_from_this();
                }
                if (!user.isValidUser(user.username, request.parameter("current_password"))) {
                    return content("Current password is incorrect")
                        ->code(Response::NOT_FOUND)
                        .appendNavBarAction({"Start", prefix + "/"})
                        .shared_from_this();
                }
                user.setPassword(request.parameter("new_password"));
                user.update();
                return content("Password updated successfully");
            } else {
                return content("User not found")
                    ->code(Response::NOT_FOUND)
                    .appendNavBarAction({"Start", prefix + "/"})
                    .shared_from_this();
                ;
            }
        });
        if (!prefix.empty()) {
            T::router().get("/", [prefix](const Request& request) {
                return redirect(prefix + "/");
            });
        }
        T::defaultHandler(Http::NotFoundHandler);
        T::finish_init();
    }
};