

#include "Submit.hpp"
#include <sstream>

using std::ostringstream;
namespace Input {
Submit::Submit(string label) : m_label(move(label)) {}
string Submit::operator()() {
    ostringstream str;
    str << R"(<input type="submit" m_id=")" << m_label << R"(" name=")" << m_label << R"(">)";
    return str.str();
}
}// namespace Input