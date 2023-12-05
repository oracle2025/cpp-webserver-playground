#include "InvoicesApplication.hpp"

#include "CrudController.hpp"
#include "Data/ScaffoldRecord.hpp"
#include "Http/RequestDispatcher.hpp"
#include "PocoWebServer.hpp"
#include "SimpleWebServer.hpp"

#include <utility>

struct Invoice : public Data::ScaffoldRecord {
    Invoice() = delete;
    explicit Invoice(const Http::Request&)
        : ScaffoldRecord(
            "invoice",
            {{"invoiceNr", HtmlInputType::TEXT},
             {"customerId", HtmlInputType::TEXT},
             {"invoiceDate", HtmlInputType::DATE},
             {"dueDate", HtmlInputType::DATE},
             {"totalAmount", HtmlInputType::TEXT},
             {"paid", HtmlInputType::TEXT}})
    {
    }
};
struct Customer : public Data::ScaffoldRecord {
    Customer() = delete;
    explicit Customer(const Http::Request&)
        : ScaffoldRecord(
            "customer",
            {{"name", HtmlInputType::TEXT},
             {"email", HtmlInputType::TEXT},
             {"phone", HtmlInputType::TEXT},
             {"address", HtmlInputType::TEXT},
             {"uid", HtmlInputType::TEXT}})
    {
    }
};

template<typename T>
struct SimpleController : public T {
    explicit SimpleController(Http::RequestHandlerList handlers)
        : m_handler(std::move(handlers))
    {
        T::defaultHandler([this](const Request& request) {
            return m_handler.handle(request);
        });
        T::finish_init();
    }

private:
    Http::RequestDispatcher m_handler;
};

int Invoices::InvoicesApplication::main(const std::vector<std::string>& args)
{
    SimpleController<PocoWebServer> server(Http::RequestHandlerList{
        make_shared<CrudController<SimpleWebServer, Invoice>>("/invoice"),
        make_shared<CrudController<SimpleWebServer, Customer>>("/customer")});
    server.start();
    waitForTerminationRequest();
    server.stop();
    return EXIT_OK;
}
