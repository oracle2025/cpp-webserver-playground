

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
    str << R"(<button type="submit" m_id=")" << m_label << R"(" title=")"
        << m_label << R"(">)" << m_label << R"(</button>)";
    return str.str();
}
} // namespace Input