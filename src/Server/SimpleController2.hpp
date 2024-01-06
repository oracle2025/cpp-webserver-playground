#pragma once


#include "WebServer.hpp"
#include "Html/Presentation.hpp"
#include "Http/RequestHandler.hpp"

class SimpleController2 {
public:
    using RequestHandler = Http::RequestHandler;
    SimpleController2(
        WebServer& T,
        std::shared_ptr<Http::RequestHandler> handler,
        std::shared_ptr<Html::Presentation> presentation)
        : m_handler(handler)
        , m_presentation(presentation)
    {
        using Http::Request;
        T.defaultHandler([this](const Request& request) {
            auto result = m_handler->handle(request);
            for (auto& action : m_actions) {
                result->appendNavBarAction(action);
            }
            return result;
        });
        T.setPresentation(m_presentation);
        T.finish_init();
    }
    void setActions(const std::vector<ActionLink>& actions)
    {
        m_actions = actions;
    }

private:
    std::shared_ptr<RequestHandler> m_handler;
    std::shared_ptr<Html::Presentation> m_presentation;
    std::vector<ActionLink> m_actions;
};



