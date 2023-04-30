
#include "Text.hpp"


#include <sstream>

using std::ostringstream;

namespace Input {
string Text::operator()()
{
    ostringstream str;
    str << R"(<label for =")" << label << R"(">)" << label
        << R"(</label><br> <input type="text" m_id=")" << label << R"(" name=")"
        << label << R"(" value=")" << value << R"(">)";
    return str.str();
}
Text::Text(string label, string value)
    : label(move(label))
    , value(move(value))
{
}


} // namespace Input