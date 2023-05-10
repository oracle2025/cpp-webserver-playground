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
    Todo() = default;

    Todo(
        string id,
        string description,
        string created_at,
        string updated_at,
        bool checked);

    string key() const override;

    bool checked = false

    static std::vector<Todo> list();

    static std::vector<std::shared_ptr<Record>> listAsPointers();

    void insert();

    void update();

    bool pop(const string& _id);

    void erase();

    void set(const string& key, const string& value);

    static void create_table();

    std::vector<string> fields() const override;
    HtmlInputType inputType(const string& field) const override;

    std::map<string, string> values() const override;

    string description() const;
private:
    string id;
    string m_description;
    string created_at;
    string updated_at;
};

string time_string();