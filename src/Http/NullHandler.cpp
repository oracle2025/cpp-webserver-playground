#include "NullHandler.hpp"
#include "Response.hpp"
namespace Http {
handler_type NullHandler = [](const Request&) {
    return nullptr;
};
}