#include "Confirm.hpp"

#include "Http/Response.hpp"
#include "Input/Form.hpp"
#include "Input/Submit.hpp"
#include "String/escape.hpp"

using std::make_shared;

shared_ptr<Http::Response> Confirm::operator()()
{
    using Http::content;
    using Input::Form;
    using Input::Submit;
    return content(Form(
                       {make_shared<Submit>("Cancel")
                            ->name("canceled")
                            .valueP("yes")
                            .buttonClass("light").shared_from_this(),
                        make_shared<Submit>("Delete " + String::escape(description))
                            ->name("confirmed")
                            .valueP("yes")
                            .buttonClass("danger").shared_from_this()},
                       prefix + "/delete?" + todo.key(),
                       "post")())
        ->title("Confirm Delete")
        .shared_from_this();
}
Confirm::Confirm(const string& prefix, const Record& todo, const string& description)
    : prefix(prefix)
    , todo(todo)
    , description(description)
{
}
