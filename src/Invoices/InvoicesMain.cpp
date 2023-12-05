#include "InvoicesApplication.hpp"
#include <iostream>
using std::cerr;
using std::endl;
int main(const int argc, const char** argv)
{
    try {
        Invoices::InvoicesApplication app;
        app.run(argc, const_cast<char**>(argv));
    } catch (Poco::Exception& exc) {
        cerr << exc.displayText() << endl;
    }
    return 0;
}