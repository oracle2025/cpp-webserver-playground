
#include "Text.hpp"


#include <sstream>

using std::ostringstream;

namespace Input {
string Text::operator()()
{
    std::string capitalized = label;
    std::transform(capitalized.begin(), capitalized.begin() + 1, capitalized.begin(), ::toupper);

    ostringstream str;
    str << R"(<label for =")" << label << R"(">)" << capitalized
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