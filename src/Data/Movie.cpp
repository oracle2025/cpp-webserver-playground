#include "Movie.hpp"

namespace Data {

MovieDefinition::MovieDefinition(MovieDefinition::RecordType d)
    : data{std::move(d)}
    , id(data.get<0>())
    , title(data.get<1>())
    , director(data.get<2>())
    , genre(data.get<3>())
    , year(data.get<4>())
    , rating(data.get<5>())
    , user_id(data.get<6>())
{
}
MovieDefinition::MovieDefinition()
    : data{"", "", "", "", "", "", ""}
    , id(data.get<0>())
    , title(data.get<1>())
    , director(data.get<2>())
    , genre(data.get<3>())
    , year(data.get<4>())
    , rating(data.get<5>())
    , user_id(data.get<6>())
{
}
MovieDefinition::MovieDefinition(const MovieDefinition& t)
    : data{t.data}
    , id(data.get<0>())
    , title(data.get<1>())
    , director(data.get<2>())
    , genre(data.get<3>())
    , year(data.get<4>())
    , rating(data.get<5>())
    , user_id(data.get<6>())
{
}
std::string MovieDefinition::description() const
{
    return title + " (" + year + ") by " + director;
}
std::string MovieDefinition::get(const KeyStringType& key) const
{
    if (key == "id") {
        return id;
    } else if (key == "title") {
        return title;
    } else if (key == "director") {
        return director;
    } else if (key == "genre") {
        return genre;
    } else if (key == "year") {
        return year;
    } else if (key == "rating") {
        return rating;
    } else if (key == "user_id") {
        return user_id;
    }
    return "";
}
void MovieDefinition::set(const KeyStringType& key, const string& value)
{
    if (key == "id") {
        id = value;
    } else if (key == "title") {
        title = value;
    } else if (key == "director") {
        director = value;
    } else if (key == "genre") {
        genre = value;
    } else if (key == "year") {
        year = value;
    } else if (key == "rating") {
        rating = value;
    } else if (key == "user_id") {
        user_id = value;
    }
}
std::vector<ColumnType> MovieDefinition::columns()
{
    static const std::vector<ColumnType> cols{
        ColumnType{"title", "VARCHAR", HtmlInputType::TEXT},
        ColumnType{"director", "VARCHAR", HtmlInputType::TEXT},
        ColumnType{"genre", "VARCHAR", HtmlInputType::TEXT},
        ColumnType{"year", "VARCHAR", HtmlInputType::TEXT},
        ColumnType{"rating", "VARCHAR", HtmlInputType::TEXT},
        ColumnType{"user_id", "VARCHAR", HtmlInputType::HIDDEN},
    };
    return cols;
}
std::string MovieDefinition::table_name()
{
    return "Movie";
}
std::vector<KeyStringType> MovieDefinition::presentableFields()
{
    return {"title", "director", "genre", "year", "rating"};
}
std::string MovieDefinition::presentableName()
{
    return "Movie";
}
} // namespace Data