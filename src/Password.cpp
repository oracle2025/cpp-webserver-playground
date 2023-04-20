
#include "Password.hpp"
#include <sstream>
#include "doctest.h"

using std::ostringstream;
namespace Input {

string Password::operator()() {
    ostringstream str;
    str << R"(<input type="password" m_id=")" << m_label << R"(" name=")" << m_label << R"(">)";
    return str.str();
}
Password::Password(string label) : m_label(move(label)) {}

TEST_CASE("Password Element") {
    CHECK(Input::Password("password")() == R"(<input type="password" m_id="password" name="password">)");
    CHECK(Input::Password("password2")() == R"(<input type="password" m_id="password2" name="password2">)");
}
}// namespace Input
