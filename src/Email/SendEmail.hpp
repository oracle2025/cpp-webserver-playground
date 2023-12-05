#pragma once

#include <string>

namespace Poco::Net {
class MailMessage;
}

namespace Email {

using std::string;

class SendEmail {
public:
    SendEmail(string server, string user, string password);
    void sendHTML(
        const string& from,
        const string& receiver,
        const string& subject,
        const string& body);

private:
    const string SMTP_SERVER;
    const string SMTP_USER;
    const string SMTP_PASSWORD;
    void send(const Poco::Net::MailMessage& message) const;
};

} // namespace Email
