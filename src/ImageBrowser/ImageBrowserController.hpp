#pragma once

#include "Http/NotFoundHandler.hpp"
#include "Http/NullHandler.hpp"
#include "Http/Request.hpp"
#include "Http/Response.hpp"

#include <filesystem>
#include <sstream>

namespace ImageBrowser {

using std::vector;

template<typename T>
struct ImageBrowserController : public T {
    ImageBrowserController(const string& prefix)
    {
        using Http::content;
        using Http::Request;
        using Http::Response;
        T::router().get(prefix + "/", [this](const Request& request) {
            using namespace std::filesystem;
            vector<string> result;
            for (auto const& dir_entry :
                 directory_iterator{CURRENT_SOURCE_DIR "/tests/images/"}) {
                result.push_back(dir_entry.path().filename());
            }
            return content(generateHtml(result));
        });
        T::router().get(prefix + "/images/", [this](const Request& request) {
            using namespace std::filesystem;
            vector<string> result;
            for (auto const& dir_entry :
                 directory_iterator{CURRENT_SOURCE_DIR "/tests/images/"}) {
                result.push_back(dir_entry.path().filename());
            }
            if (std::find(result.begin(), result.end(), request.query())
                == result.end()) {
                return Http::NotFoundHandler(request);
            }
            return Response::create()
                ->sendFile(
                    CURRENT_SOURCE_DIR "/tests/images/" + request.query())
                .mimetype("image/jpeg")
                .shared_from_this();
        });
        // T::setPresentation(m_presentation);
        T::defaultHandler(Http::NotFoundHandler);
        T::finish_init();
    }
    static string generateHtml(const vector<string>& images);
};
template<typename T>
string ImageBrowserController<T>::generateHtml(const vector<string>& images)
{
    std::ostringstream str;
    for (auto& image : images) {
        str << R"(<img width=128 height=128 src="/imagebrowser/images/?)" << image
            << R"("><br />)"
               "\n";
    }
    return str.str();
}

} // namespace ImageBrowser