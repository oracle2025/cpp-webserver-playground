#include "join.hpp"

#include <numeric>
#include <string>
#include <vector>
namespace String {

using std::accumulate;
string join(vector<string> const& strings, string delim)
{
    if (strings.empty()) {
        return {};
    }

    return accumulate(
        strings.begin() + 1,
        strings.end(),
        strings[0],
        [&delim](string x, string y) { return x + delim + y; });
}
} // namespace String