
#include "ProtoServerApplication.hpp"

#include "CrudController.hpp"
#include "Data/ScaffoldRecord.hpp"
#include "Html/Presentation.hpp"
#include "PocoWebServer.hpp"
#include "RequestDispatcher.hpp"
#include "SimpleController.hpp"
#include "SimpleWebServer.hpp"
#include "String/capitalize.hpp"

#include <doctest.h>
#include <spdlog/spdlog.h>

#include <filesystem>
#include <fstream>
#include <sstream>
namespace fs = std::filesystem;

namespace ProtoServer {

using std::string;

static string extractNameFromPath(const fs::path& path)
{
    return path.stem().string();
}

static vector<std::tuple<KeyStringType, HtmlInputType>> extractFieldsFromCsv(
    std::istream& ifs)
{
    vector<std::tuple<KeyStringType, HtmlInputType>> fields;
    string line;
    std::getline(ifs, line);
    std::istringstream iss(line);
    string field;
    while (std::getline(iss, field, ',')) {
        fields.push_back({field, HtmlInputType::TEXT});
    }
    return fields;
}

TEST_CASE("parse Fields from CSV Header")
{
    std::istringstream oss("header1,header2,header3\n");
    auto fields = extractFieldsFromCsv(oss);
    CHECK(fields.size() == 3);
}

static vector<std::tuple<KeyStringType, HtmlInputType>> extractFieldsFromCsv(
    const fs::path& path)
{
    std::ifstream ifs(path);
    return extractFieldsFromCsv(ifs);
}

class ProtoCrudController
    : public CrudController<SimpleWebServer, Data::ScaffoldRecord> {
public:
    using FieldsType = Data::ScaffoldRecord::FieldsType;
    ProtoCrudController(string name, FieldsType fields)
        : CrudController("/" + name)
        , m_name(std::move(name))
        , m_fields(std::move(fields))
    {
    }
    Data::ScaffoldRecord makeRecord(const Request& request) override
    {
        Data::ScaffoldRecord record(m_name, m_fields);
        return record;
    }

private:
    string m_name;
    FieldsType m_fields;
};

static std::shared_ptr<Http::RequestHandler> createCsvRecordHandler(
    const fs::path& path)
{
    return std::make_shared<ProtoCrudController>(
        extractNameFromPath(path), extractFieldsFromCsv(path));
}

int ProtoServerApplication::main(const vector<string>& args)
{
    using Presentation = Html::Presentation;
    using RequestDispatcher = Http::RequestDispatcher;
    // List Files matching *.csv in CONFIG_DIR

    const auto folder = CONFIG_DIR "/proto";

    Http::RequestHandlerList handlers;
    std::vector<ActionLink> actions;

    for (const auto& entry : fs::directory_iterator(folder)) {
        spdlog::info("Folder: {}", entry.path().string());
        handlers.push_back(createCsvRecordHandler(entry.path()));
        actions.push_back(
            {String::capitalize(extractNameFromPath(entry.path())),
             "/" + extractNameFromPath(entry.path()) + "/"});
    }

    SimpleController<PocoWebServer> server(
        make_shared<RequestDispatcher>(handlers),
        std::make_shared<Presentation>());
    server.setActions(actions);

    server.start();
    waitForTerminationRequest();
    server.stop();
    return EXIT_OK;
}
} // namespace ProtoServer