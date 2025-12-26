#include "SignupController.hpp"

#include "Data/User.hpp"
#include "Http/NullHandler.hpp"
#include "Http/Request.hpp"
#include "Http/Response.hpp"
#include "Http/Session.hpp"
#include "Input/Form.hpp"
#include "Input/Password.hpp"
#include "Input/Submit.hpp"
#include "Input/Text.hpp"
#include "String/createRandomUUID.hpp"
namespace Signup {
using Http::content;
using Http::redirect;
using Request = Http::Request;
using Session = Http::Session;
using Response = Http::Response;
void SignupController::initialize(const string& prefix, Http::Router& router)
{
    using namespace Input;
    router.get(prefix + "/", [prefix](const Request& request) {
        auto form = make_shared<Form>(
            vector<ElementPtr>{
                make_shared<Text>("username"),
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
    router.post(prefix + "/submit", [prefix](const Request& request) {
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
        // generate API key for the new user
        static const KeyStringType API_KEY_FIELD = "api_key";
        user.set(API_KEY_FIELD, String::createRandomUUID());
        user.insert();

        auto response = redirect("/")
                            ->alert("User created", Html::AlertType::SUCCESS)
                            .shared_from_this();
        Session(request).clearSession();
        Session(request).current(*response).login(user);
        return response;
    });
}
} // namespace Signup