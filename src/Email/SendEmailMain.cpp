

#include "SendEmail.hpp"

#include <algorithm>
#include <sstream>

#include <cstdlib>
#include <fstream>

std::string read_string_from_file(const std::string& filename)
{
    const std::ifstream input_stream(filename, std::ios_base::binary);

    if (input_stream.fail()) {
        throw std::runtime_error("Failed to open file");
    }

    std::stringstream buffer;
    buffer << input_stream.rdbuf();

    auto result = buffer.str();
    result.erase(
        remove_if(result.begin(), result.end(), isspace), result.end());
    return result;
}

int main()
{
    const auto SMTP_SERVER = getenv("SMTP_SERVER");
    const auto SMTP_USER = getenv("SMTP_USER");
    const auto SMTP_PASSWORD = getenv("SMTP_PASSWORD");

    const auto SMTP_SERVER_2 = read_string_from_file(CONFIG_DIR "/smtp/server");
    const auto SMTP_USER_2 = read_string_from_file(CONFIG_DIR "/smtp/user");
    const auto SMTP_PASSWORD_2
        = read_string_from_file(CONFIG_DIR "/smtp/password");

    Email::SendEmail sendEmail(SMTP_SERVER, SMTP_USER, SMTP_PASSWORD);
    sendEmail.sendHTML(SMTP_USER, "richard.spindler@gmail.com", "Test", "");
    return EXIT_SUCCESS;

}
