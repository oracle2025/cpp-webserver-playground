
#include "Http/Response.hpp"
#include "Page.hpp"
#include "Presentation.hpp"
#include "doctest.h"

TEST_CASE("Create Response")
{
    auto response = Http::content("Hello World");
}

TEST_CASE("Render Response")
{
    auto response = Http::content("Hello World");
    auto presentation = Html::Presentation("Todo List");
    auto rendered = presentation.render(*response);
    //lCHECK(rendered == "<html>Hello World</html>");
}

TEST_CASE("Render Response as Page")
{
    auto response = Http::content("Hello World");
    auto page = Page("Title", response->content());
    auto rendered = page.get();
    CHECK(rendered == R"(<!doctype html><html lang="de"><meta charset="utf-8"><meta name="viewport" content="width=device-width, initial-scale=1.0"><head><title>Title</title></head><h1>Title</h1><body>Hello World</body></html>)");
}