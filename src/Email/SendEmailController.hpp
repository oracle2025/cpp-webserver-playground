#pragma once

#include "Email/SendEmail.hpp"
#include "FileSystem/ReadFromFile.hpp"
#include "Filter/ByOwner.hpp"
#include "Html/List.hpp"
#include "Http/Request.hpp"
#include "Http/Response.hpp"
#include "NullHandler.hpp"
#include "WebServer.hpp"

namespace Email {
template<typename T>
class SendEmailController : public T {
public:
    SendEmailController(const string& prefix)
    {
        using Http::Request;
        static_assert(
            std::is_base_of<WebServer, T>::value,
            "T not derived from WebServer");
        T::router().get(prefix + "/send", [](const Request& request) {
            Filter::ByOwner record(request);
            auto columns = record.presentableFieldsImpl();

            Html::List list(record.listAsPointers(), columns);
            const auto SMTP_SERVER
                = FileSystem::ReadFromFile::read_string_from_file(
                    CONFIG_DIR "/smtp/server");
            const auto SMTP_USER
                = FileSystem::ReadFromFile::read_string_from_file(CONFIG_DIR
                                                                  "/smtp/user");
            const auto SMTP_PASSWORD
                = FileSystem::ReadFromFile::read_string_from_file(
                    CONFIG_DIR "/smtp/password");

            Email::SendEmail sendEmail(SMTP_SERVER, SMTP_USER, SMTP_PASSWORD);
            sendEmail.sendHTML(
                SMTP_USER, "richard.spindler@gmail.com", "Test", list());
            return Http::content("send_email")
                ->appendNavBarAction({"Start", "/"})
                .title("Send Email")
                .shared_from_this();
        });
        T::defaultHandler(Http::NullHandler);
        T::finish_init();
    }
    std::string generateEmailBody(const Http::Request& request)
    {
        Filter::ByOwner record(request);
        const auto columns = record.presentableFieldsImpl();

        Html::List list(record.listAsPointers(), columns);
        return list();
    }
};
} // namespace Email