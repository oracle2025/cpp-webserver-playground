#include "Form.hpp"
#include "Password.hpp"
#include "Text.hpp"
#include "doctest.h"

TEST_CASE("Text Element")
{
    CHECK(
        Input::Text("username")()
        == R"(<label for="username">Username</label><br> <input type="text" id="username" name="username" value="">)");
}

TEST_CASE("Password Element")
{
    CHECK(
        Input::Password("password")()
        == R"(<label for="password">Password</label><br> <input type="password" id="password" name="password" value="">)");
    CHECK(
        Input::Password("password2")()
        == R"(<label for="password2">Password2</label><br> <input type="password" id="password2" name="password2" value="">)");
}

TEST_CASE("Form Data")
{
    using Input::Form;
    using Input::Password;
    using Input::Text;
    auto form = Input::Form(
        {std::make_shared<Text>("user"), std::make_shared<Password>("pass")},
        "/login",
        "POST");

    CHECK(form.data() == "user=&pass=&");

    form.set("user", "admin");
    form.set("pass", "Adm1n!");

    CHECK(form.data() == "user=admin&pass=Adm1n!&");
}
