#include "PasswordChangeController.hpp"

#include "Data/User.hpp"
#include "Http/Request.hpp"
#include "Http/Response.hpp"
#include "Http/Router.hpp"
#include "Http/Session.hpp"
#include "Input/Form.hpp"
#include "Input/Password.hpp"
#include "Input/Submit.hpp"

#include <string>

using Http::content;
using Http::redirect;
using Http::Request;
using Http::Response;
using Http::Session;

void PasswordChangeController::initialize(
    const string& prefix, Http::Router& router)
{
    router.get(prefix + "/", [prefix](const Request& request) {
        using namespace Input;
        Data::User user;
        if (user.pop(Session(request).userId())) { // current session user
            auto form
                = std::make_shared<Form>(
                      vector<ElementPtr>{
                          make_shared<Password>("current_password"),
                          make_shared<Password>("new_password"),
                          make_shared<Password>("confirm_password")},
                      string(prefix + "/update"),
                      "post")
                      ->appendElement(make_shared<Submit>("Update Password"))
                      .shared_from_this();
            return content((*form)())
                ->form(form)
                .title("Change Password")
                .shared_from_this();
        } else {
            return content("User not found")
                ->code(Response::NOT_FOUND)
                .shared_from_this();
            ;
        }
    });
    router.post(prefix + "/update", [prefix](const Request& request) {
        Data::User user;
        if (user.pop(Session(request).userId())) {
            if (request.parameter("new_password")
                != request.parameter("confirm_password")) {
                return content("Passwords do not match")
                    ->code(Response::NOT_FOUND)
                    .shared_from_this();
            }
            if (!user.isValidUser(
                    user.username,
                    request.parameter("current_password"),
                    user)) {
                return content("Current password is incorrect")
                    ->code(Response::NOT_FOUND)
                    .shared_from_this();
            }
            user.setPassword(request.parameter("new_password"));
            user.update();
            return content("Password updated successfully");
        } else {
            return content("User not found")
                ->code(Response::NOT_FOUND)
                .shared_from_this();
            ;
        }
    });
    if (!prefix.empty()) {
        router.get("/", [prefix](const Request& request) {
            return redirect(prefix + "/");
        });
    }
}
