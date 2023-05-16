#include "repeat.hpp"
namespace String {
string repeat(const string& str, const string& delim, size_t times) {
    string result;
    for (size_t i = 0; i < times; ++i) {
        result += str;
        if (i < times - 1) {
            result += delim;
        }
    }
    return result;
}

}