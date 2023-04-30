#include "doctest.h"

#include "Text.hpp"
#include "Password.hpp"

TEST_CASE("Text Element")
{
    CHECK(
        Input::Text("username")()
        == R"(<label for ="username">username</label><br> <input type="text" m_id="username" name="username" value="">)");
}

TEST_CASE("Password Element")
{
    CHECK(
        Input::Password("password")()
        == R"(<input type="password" m_id="password" name="password">)");
    CHECK(
        Input::Password("password2")()
        == R"(<input type="password" m_id="password2" name="password2">)");
}