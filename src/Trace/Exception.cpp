
#include "Exception.hpp"
#include "trace.hpp"

#include <Poco/Exception.h>
#include <filesystem>

namespace Trace {
Exception::Exception(
    const std::string& message, const char* filename, unsigned int line)
    : std::runtime_error(message)
    , filename(filename)
{
    using path = std::filesystem::path;
    stacktrace.load_here(32);
    // Skipping 4 first frames because they will always show this constructor
    stacktrace.skip_n_firsts(4);
    _message
        = path(filename).filename().string() + ":" + std::to_string(line) + " : " + message;
}
void Exception::printStackTrace(std::ostream& out) const
{
    backward::Printer p;
    p.print(stacktrace, out);
}

void backtrace(const std::exception_ptr& ex, std::ostream& out, int level)
{
    try {
        try {
            std::rethrow_exception(ex);
        } catch (const Poco::Exception& e) {
            out << level << ": " << e.displayText() << std::endl;
            std::rethrow_if_nested(e);
        } catch (const Trace::Exception& e) {
            out << level << ": " << e.what() << std::endl;
            e.printStackTrace(out);
            std::rethrow_if_nested(e);
        } catch (const std::exception& e) {
            out << level << ": " << e.what() << std::endl;
            std::rethrow_if_nested(e);
        } catch (...) {
            out << level << ": " << "Unknown exception" << std::endl;
            std::rethrow_if_nested(std::current_exception());
        }
    } catch (...) {
        backtrace(std::current_exception(), out, level + 1);
    }
}

} // namespace Trace