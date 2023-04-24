#pragma once

#include "Record.hpp"

#include <map>
#include <memory>
#include <string>
#include <vector>

namespace Poco::Data {
class Session;
}

extern Poco::Data::Session* g_session;

using std::string;

struct Todo : public Record {
    Todo()
    {
    }

    Todo(
        string id,
        string description,
        string created_at,
        string updated_at,
        bool checked)
        : m_id(std::move(id))
        , description(std::move(description))
        , created_at(std::move(created_at))
        , updated_at(std::move(updated_at))
        , checked(checked)
    {
    }

    string id() const override
    {
        return m_id;
    }

    string m_id;
    string description;
    string created_at;
    string updated_at;
    bool checked;

    static std::vector<Todo> list();

    static std::vector<std::shared_ptr<Record>> listAsPointers();
    ;

    void insert();

    void update();

    bool pop(const string& _id);

    void erase();

    void set(const string& key, const string& value);

    static void create_table();

    std::vector<string> fields() const override
    {
        return {"m_id", "description", "created_at", "updated_at", "checked"};
    }
    HtmlInputType inputType(const string& field) const override
    {
        if (field == "checked") {
            return HtmlInputType::CHECKBOX;
        }
        return HtmlInputType::TEXT;
    }

    std::map<string, string> values() const override;
};
