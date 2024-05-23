#include "MovieController.hpp"

#include "Data/AsJson.hpp"
#include "Data/Record.hpp"
#include "Response.hpp"
#include "Template/BaseTemplate.hpp"

#include <utility>
using Http::content;

using BaseTemplate = Template::BaseTemplate;

MovieController::MovieController(
    const CrudController::string& prefix,
    CrudController::make_record_func makeRecordFunc)
    : CrudController(prefix, std::move(makeRecordFunc))
{
}
shared_ptr<MovieController::Response> MovieController::listRecords(
    const CrudController::Request& request)
{
    auto record = m_makeRecord(request);
    auto data = nlohmann::json::object();
    auto movies = nlohmann::json::array();
    for (auto& line : record->listAsPointers()) {
        movies.push_back(Data::AsJson::fromRecord(*line));
    }
    data["movies"] = movies;
    return content(BaseTemplate(TEMPLATE_DIR "/movie/list.html").render(data))
        ->appendAction(
            {"Create new " + record->presentableName(), prefix() + "/new"})
        .title(record->presentableName() + " List")
        .shared_from_this();
}
