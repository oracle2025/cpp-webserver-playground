#pragma once

#include <iosfwd>
#include <memory>
#include <nlohmann/json.hpp>

namespace Template {

class BaseTemplate {
public:
    BaseTemplate(const std::string& name);
     ~BaseTemplate();
    std::string operator()();
    std::string render(const nlohmann::json& data);
private:
    struct BaseTemplateImpl;
    std::unique_ptr<BaseTemplateImpl> impl_;
};

} // Template

