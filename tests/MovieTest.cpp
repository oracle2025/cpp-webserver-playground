#include "doctest.h"

#include <Poco/Data/SQLite/Connector.h>
#include <Poco/Data/Session.h>
using Poco::Data::Session;

#include "Data/Migrations.hpp"
#include "Data/Movie.hpp"

TEST_CASE("movie")
{
    Poco::Data::SQLite::Connector::registerConnector();
    Session session("SQLite", ":memory:");
    g_session = &session;

    Data::MigrationsLatest m;
    m.perform();

    using Data::Movie;

    Movie movie{Movie::RecordType{
        "0123", "The Matrix", "Wachowski", "Sci-Fi", "1999", "8.7", ""}};
    movie.insert();
    auto id = movie.key();

    static const KeyStringType TITLE_FIELD = "title";
        static const KeyStringType DIRECTOR_FIELD = "director";
        static const KeyStringType GENRE_FIELD = "genre";
        static const KeyStringType YEAR_FIELD = "year";
        static const KeyStringType RATING_FIELD = "rating";
        static const KeyStringType USER_ID_FIELD = "user_id";
    CHECK_EQ(movie.get(TITLE_FIELD), "The Matrix");
    CHECK_EQ(movie.get(DIRECTOR_FIELD), "Wachowski");
    CHECK_EQ(movie.get(GENRE_FIELD), "Sci-Fi");
    CHECK_EQ(movie.get(YEAR_FIELD), "1999");
    CHECK_EQ(movie.get(RATING_FIELD), "8.7");
    CHECK_EQ(movie.get(USER_ID_FIELD), "");

    SUBCASE("list")
    {
        auto result = movie.list();
        CHECK(result.size() == 1);
        CHECK(result[0].key() == id);
    }
    SUBCASE("pop")
    {
        Movie t;
        t.pop(id);
        CHECK_EQ(t.get("title"), "The Matrix");
        CHECK_EQ(t.get("director"), "Wachowski");
        CHECK_EQ(t.get("genre"), "Sci-Fi");
        CHECK_EQ(t.get("year"), "1999");
        CHECK_EQ(t.get("rating"), "8.7");
        CHECK_EQ(t.get("user_id"), "");
        CHECK(t.description() == "The Matrix (1999) by Wachowski");
    }
    SUBCASE("update")
    {
        Movie t;
        t.pop(id);
        t.set("genre", "Retro");
        t.update();
        {
            Movie t2;
            t2.pop(id);
            CHECK(t2.get("genre") == "Retro");
        }
    }
    SUBCASE("erase")
    {
        Movie t;
        t.pop(id);
        t.erase();
        CHECK(t.list().size() == 0);
    }
}