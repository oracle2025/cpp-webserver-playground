#include "capitalize.hpp"

namespace String {

string capitalize(string str)
{
    transform(str.begin(), str.begin() + 1, str.begin(), ::toupper);
    return str;
}

} // namespace String