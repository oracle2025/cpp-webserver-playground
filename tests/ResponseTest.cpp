#include "Footer.hpp"
#include "Header.hpp"
#include "Http/Response.hpp"
#include "Page.hpp"
#include "Presentation.hpp"
#include "doctest.h"

TEST_CASE("Create Response")
{
    auto response = content("Hello World");
}

TEST_CASE("Render Response")
{
    auto response = content("Hello World");
    auto presentation = Presentation(Header("<html>"), Footer("</html>"));
    auto rendered = presentation.render(response->content());
    CHECK(rendered == "<html>Hello World</html>");
}

TEST_CASE("Render Response as Page")
{
    auto response = content("Hello World");
    auto page = Page("Title", response->content());
    auto rendered = page.get();
    CHECK(rendered == R"(<!doctype html><html lang="de"><meta charset="utf-8"><meta title="viewport" content="width=device-width, initial-scale=1.0"><head><title>Title</title></head><h1>Title</h1><body>Hello World</body></html>)");
}