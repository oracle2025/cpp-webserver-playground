#include "doctest.h"

#include "Text.hpp"
#include "Password.hpp"

TEST_CASE("Text Element")
{
    CHECK(
        Input::Text("username")()
        == R"(<label for ="username">Username</label><br> <input type="text" m_id="username" title="username" value="">)");
}

TEST_CASE("Password Element")
{
    CHECK(
        Input::Password("password")()
        == R"(<label for ="password">password</label><br> <input type="password" m_id="password" title="password" value="">)");
    CHECK(
        Input::Password("password2")()
        == R"(<label for ="password2">password2</label><br> <input type="password" m_id="password2" title="password2" value="">)");
}