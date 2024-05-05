#include "doctest.h"
#include <functional>

struct ReferenceCounting {
    ReferenceCounting()
    {
        references++;
    }
    ~ReferenceCounting()
    {
        references--;
    }
    static int references;
};
int ReferenceCounting::references = 0;

TEST_CASE("Test Custom Reference Counter")
{
    CHECK_EQ(ReferenceCounting::references, 0);
    {
        ReferenceCounting first;
        CHECK_EQ(ReferenceCounting::references, 1);
        ReferenceCounting second;
        CHECK_EQ(ReferenceCounting::references, 2);
    }
    CHECK_EQ(ReferenceCounting::references, 0);
}

TEST_CASE("Check if Lambda Deletes Shared Ptr")
{
    CHECK_EQ(ReferenceCounting::references, 0);
    std::function<int()> myFunction;
    {
        auto first = std::make_shared<ReferenceCounting>();
        CHECK_EQ(ReferenceCounting::references, 1);
        myFunction = [first]() { return 12; };
    }
    CHECK_EQ(ReferenceCounting::references, 1);
    myFunction = [](){return 13;};
    CHECK_EQ(ReferenceCounting::references, 0);
}