#pragma once

#include "Http/NotFoundHandler.hpp"
#include "Http/NullHandler.hpp"
#include "Http/Request.hpp"
#include "Http/Response.hpp"
#include "Trace/trace.hpp"
#include "stb/stb_image.h"
#include "stb/stb_image_resize.h"
#include "stb/stb_image_write.h"

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
        T::router().get(prefix + "/thumbs/", [this](const Request& request) {
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
            auto image_file_name
                = CURRENT_SOURCE_DIR "/tests/images/" + request.query();
            const char* home = getenv("HOME");
            if (home == nullptr) {
                TRACE_THROW("$HOME not set, could not create cache folder");
            }
            std::filesystem::path cache_path = std::string(home)
                + "/.imagebrowser/cache" CURRENT_SOURCE_DIR "/tests/images/"
                + request.query();
            auto cache_folder = cache_path.parent_path();
            if(!std::filesystem::create_directories(cache_folder)) {
                TRACE_THROW("Could not create cache folder");
            }
            if (exists(cache_path)) {
                image_file_name = cache_path;
            } else {
                int width, height, channels;
                unsigned char* data = stbi_load(
                    image_file_name.c_str(), &width, &height, &channels, 0);
                if (data == nullptr) {
                    return Http::NotFoundHandler(request);
                }
                unsigned char* thumb = new unsigned char[128 * 128 * channels];
                stbir_resize_uint8(
                    data, width, height, 0, thumb, 128, 128, 0, channels);

                stbi_write_jpg(
                    cache_path.c_str(), 128, 128, channels, thumb, 80);
                delete[] thumb;
                stbi_image_free(data);
                image_file_name = cache_path;
            }
            return Response::create()
                ->sendFile(image_file_name)
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
        str << R"(<img width=128 height=128 src="/imagebrowser/images/?)"
            << image
            << R"("><br />)"
               "\n";
    }
    return str.str();
}

} // namespace ImageBrowser