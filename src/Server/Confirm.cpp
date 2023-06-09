#include "Confirm.hpp"

#include "Http/Response.hpp"
#include "Input/Form.hpp"
#include "Input/Submit.hpp"
shared_ptr<Http::Response> Confirm::operator()()
{
    using Http::content;
    using Input::Form;
    using Input::Submit;
    return content(Form(
                       {Submit("Cancel")
                            .name("canceled")
                            .value("yes")
                            .buttonClass("light")(),
                        Submit("Delete " + description)
                            .name("confirmed")
                            .value("yes")
                            .buttonClass("danger")()},
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
