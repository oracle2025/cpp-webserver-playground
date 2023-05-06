

#include "Submit.hpp"

#include <sstream>

using std::ostringstream;
namespace Input {
Submit::Submit(string label)
    : m_label(move(label))
{
}
string Submit::operator()()
{
    ostringstream str;
    str << R"(<button type="submit" id=")" << m_label << R"(" name=")"
        << m_label << R"(">)" << m_label << R"(</button>)";
    return str.str();
}
} // namespace Input