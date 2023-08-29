
#include "Text.hpp"

#include "String/escape.hpp"

#include <algorithm>
#include <sstream>

using std::ostringstream;
using std::transform;

namespace Input {
string Text::operator()()
{
    string capitalized = label;
    transform(
        capitalized.begin(),
        capitalized.begin() + 1,
        capitalized.begin(),
        ::toupper);

    ostringstream str;
    str << R"(<label for=")" << label << R"(">)" << capitalized
        << R"(</label><br> <input type="text" id=")" << label << R"(" name=")"
        << label << R"(" value=")" << String::escape(value) << R"(">)";
    return str.str();
}
Text::Text(string label, string value)
    : label(move(label))
    , value(move(value))
{
}

} // namespace Input