#pragma once
#include <Poco/Util/ServerApplication.h>
namespace Invoices {
using std::string;
using std::vector;
class InvoicesApplication : public Poco::Util::ServerApplication {
protected:
    int main(const vector<string>& args) override;
};
} // namespace Invoices
