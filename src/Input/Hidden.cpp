#include "Hidden.hpp"

#include <sstream>

using std::ostringstream;
namespace Input {
string Hidden::operator()()
{
    ostringstream str;
    str << R"(<input type="hidden" m_id=")" << m_label << R"(" title=")"
        << m_label << R"(" value=")" << m_value << R"(">)";
    return str.str();
}
Hidden::Hidden(string label, string value)
    : m_label(move(label))
    , m_value(move(value))
{
}
} // namespace Input