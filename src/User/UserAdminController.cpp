#include "UserAdminController.hpp"

#include "Data/User.hpp"
#include "Input/Form.hpp"
#include "Input/Password.hpp"
#include "Input/Select.hpp"
#include "Input/Submit.hpp"
#include "Input/Text.hpp"
#include "Request.hpp"
#include "Response.hpp"
#include "Template/BaseTemplate.hpp"

#include <nlohmann/json.hpp>

#include <utility>

using Http::content;
using Http::redirect;

using Template::BaseTemplate;

UserAdminController::UserAdminController(
    const CrudController::string& prefix,
    CrudController::make_record_func makeRecordFunc)
    : CrudController(prefix, std::move(makeRecordFunc))
{
    // User Edit must not change password
    // Only Editable filed should be role!
    // Password Reset must not change user
}
shared_ptr<CrudController::Response> UserAdminController::newRecord(
    const Request& request)
{
    using namespace Input;
    auto form = std::make_shared<Form>(
        vector<ElementPtr>{
            std::make_shared<Text>("username"),
            std::make_shared<Password>("password"),
            std::make_shared<Password>("confirm_password"),
            // std::make_shared<Text>("role"), // Use List for role
            std::make_shared<Select>(
                "role",
                std::vector<std::string>{
                    "user", "employer", "admin"}), // Use List for role
            std::make_shared<Submit>("Create User")},
        "/user/create",
        "post");
    return content((*form)())
        ->title("Create User")
        .form(form)
        .shared_from_this();
}
shared_ptr<CrudController::Response> UserAdminController::createRecord(
    const Request& request)
{
    Data::User user;
    // assert prefix() == "/user"
    const auto username = request.parameter("username");
    if (username == "admin" || findUser(*g_session, username, user)) {
        return redirect(prefix() + "/")
            ->alert("User already exists", Html::AlertType::INFO)
            .shared_from_this();
    }
    if (request.parameter("password")
        != request.parameter("confirm_password")) {
        return redirect(prefix() + "/")
            ->alert("Passwords do not match", Html::AlertType::WARNING)
            .shared_from_this();
    }
    // Username must not be empty
    // Password must not be empty
    user.username = username;
    user.setPassword(request.parameter("password"));
    user.set("role", request.parameter("role"));
    user.insert();

    auto response = redirect("/user/")
                        ->alert("User created", Html::AlertType::SUCCESS)
                        .shared_from_this();
    return response;
}
