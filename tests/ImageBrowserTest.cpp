#include "ImageBrowser/ImageBrowserController.hpp"
#include "Server/TestServer.hpp"
#include "doctest.h"

#include <filesystem>

TEST_CASE("Image Browser")
{
    using ImageBrowser::ImageBrowserController;
    ImageBrowserController<TestServer> w("/imagebrowser");

    SUBCASE("Index")
    {
        auto response = w.handle({"/imagebrowser/"});
        CHECK(response->content() == R"(<img width=128 height=128 src="/imagebrowser/images/?P1090403.JPG"><br />
<img width=128 height=128 src="/imagebrowser/images/?P1090417.JPG"><br />
<img width=128 height=128 src="/imagebrowser/images/?P1090399.JPG"><br />
<img width=128 height=128 src="/imagebrowser/images/?P1090392.JPG"><br />
)");
    }
    SUBCASE("List Dir")
    {
        using namespace std::filesystem;
        using std::vector;

        vector<string> result;
        for (auto const& dir_entry :
             directory_iterator{CURRENT_SOURCE_DIR "/tests/images/"}) {
            result.push_back(dir_entry.path().filename());
        }

        CHECK(result.size() == 4);
        CHECK_EQ(result[0], "P1090403.JPG");
    }
    SUBCASE("Generate HTML")
    {
        using std::vector;
        auto input = vector<string>{"P1090403.JPG", "P1090404.JPG"};
        auto expected = R"(<img width=128 height=128 src="/imagebrowser/images/?P1090403.JPG"><br />
<img width=128 height=128 src="/imagebrowser/images/?P1090404.JPG"><br />
)";
        CHECK_EQ(ImageBrowserController<TestServer>::generateHtml(input), expected);
    }

    /* List Dir
     * Generate HTML
     * Scale Images
     * */
}
