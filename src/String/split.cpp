#include "split.hpp"

namespace String {

vector<string> split(string input, const string& delim)
{
    // split string input by delim
    vector<string> result;
    size_t pos = 0;
    string token;
    while ((pos = input.find(delim)) != string::npos) {
        token = input.substr(0, pos);
        result.push_back(token);
        input.erase(0, pos + delim.length());
    }
    result.push_back(input);
    return result;
}
} // namespace String