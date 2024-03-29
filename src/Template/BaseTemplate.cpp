#include "BaseTemplate.hpp"

#include "Trace/trace.hpp"

#include <inja.hpp>

namespace Template {
struct BaseTemplate::BaseTemplateImpl {
    inja::Template tpl;
};
BaseTemplate::BaseTemplate(const std::string& name)
    : impl_(new BaseTemplateImpl)
{
    try {
        inja::Environment env;
        impl_->tpl = env.parse_template(name);
    } catch (...) {
        TRACE_RETHROW("Could not parse template");
    }
}
BaseTemplate::~BaseTemplate() = default;

std::string BaseTemplate::operator()()
{
    return render({});
}
std::string BaseTemplate::render(const nlohmann::json& data)
{
    std::ostringstream out;
    try {
        inja::Environment env;
        out << env.render(impl_->tpl, data);
    } catch (...) {
        TRACE_RETHROW("Could not render template");
    }
    return out.str();
}
} // namespace Template