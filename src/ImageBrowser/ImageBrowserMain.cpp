#include "ImageBrowserApplication.hpp"
#include <iostream>
using std::cerr;
using std::endl;

int main()
{
    try {
        ImageBrowser::ImageBrowserApplication app;
        static char buffer[MAX_INPUT];
        strncpy(buffer, "app", MAX_INPUT);
        char* argv[] = {buffer};
        app.run(1, argv);
    } catch (Poco::Exception& exc) {
        cerr << exc.displayText() << endl;
    }
    return 0;
}