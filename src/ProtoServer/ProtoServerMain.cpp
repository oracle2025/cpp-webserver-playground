#include "ProtoServerApplication.hpp"
#include "Trace/trace.hpp"
#include "spdlog/spdlog.h"

#include <cstdlib>
#include <sstream>

int main(int argc, char** argv)
{
    /*
     * Loads a List of CSV files from a folder
     * and creates a CRUD based on the data
     */
    try {
        ProtoServer::ProtoServerApplication app;
        return app.run(argc, argv);
    } catch (...) {
        std::ostringstream str;
        Trace::backtrace(std::current_exception(), str);
        spdlog::error("Exception: {}", str.str());
        return EXIT_FAILURE;
    }
    return EXIT_SUCCESS;
}