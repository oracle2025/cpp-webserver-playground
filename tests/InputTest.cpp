#include "doctest.h"

#include "Text.hpp"
#include "Password.hpp"

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