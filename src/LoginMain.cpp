#include "LoginServerApplication.hpp"
#include <iostream>
using std::cerr;
using std::endl;

int main(int argc, char** argv)
{
    try {
        LoginServerApplication app;
        return app.run(argc, argv);
    } catch (Poco::Exception& exc) {
        cerr << exc.displayText() << endl;
        return EXIT_FAILURE;
    }
}