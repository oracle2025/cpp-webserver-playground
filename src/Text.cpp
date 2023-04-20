
#include "Text.hpp"
#include <sstream>
#include "doctest.h"

using std::ostringstream;

namespace Input {
string Text::operator()() {
    ostringstream str;
    str << R"(<label for =")"
        << label << R"(">)" << label << R"(</label><br> <input type="text" m_id=")"
        << label << R"(" name=")"
        << label << R"(" value=")" << value << R"(">)";
    return str.str();
}
Text::Text(string label, string value) : label(move(label)), value(move(value)) {}

TEST_CASE("Text Element") {
    CHECK(Input::Text("username")() == R"(<input type="text" m_id="username" name="username">)");
}
}// namespace Input