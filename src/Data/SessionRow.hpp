#pragma once
#include "Data/RecordImpl.hpp"
#include "Http/SessionData.hpp"

#include <Poco/Tuple.h>
namespace Data {

struct SessionRowDefinition {
    using string = std::string;
    using RecordType = Poco::Tuple<
        string,
        bool,
        string,
        string,
        string,
        string,
        string,
        string,
        bool,
        string,
        string>;
    RecordType data;
    string& id;
    bool& isLoggedIn;
    string& userId;
    string& userName;
    string& createdAt;
    string& lastUsedAt;
    string& path;
    string& userAgent;
    bool &hasAlert;
    string& alert;
    string& alertType;
    explicit SessionRowDefinition(RecordType d)
        : data{std::move(d)}
        , id(data.get<0>())
        , isLoggedIn(data.get<1>())
        , userId(data.get<2>())
        , userName(data.get<3>())
        , createdAt(data.get<4>())
        , lastUsedAt(data.get<5>())
        , path(data.get<6>())
        , userAgent(data.get<7>())
        , hasAlert(data.get<8>())
        , alert(data.get<9>())
        , alertType(data.get<10>())
    {
    }
    SessionRowDefinition()
                : data{"", false, "", "", "", "", "", "", false, "", ""}
                , id(data.get<0>())
                , isLoggedIn(data.get<1>())
                , userId(data.get<2>())
                , userName(data.get<3>())
                , createdAt(data.get<4>())
                , lastUsedAt(data.get<5>())
                , path(data.get<6>())
                , userAgent(data.get<7>())
                , hasAlert(data.get<8>())
                , alert(data.get<9>())
                , alertType(data.get<10>())
        {
        }
        SessionRowDefinition(const SessionRowDefinition& t)
                : data{t.data}
                , id(data.get<0>())
                , isLoggedIn(data.get<1>())
                , userId(data.get<2>())
                , userName(data.get<3>())
                , createdAt(data.get<4>())
                , lastUsedAt(data.get<5>())
                , path(data.get<6>())
                , userAgent(data.get<7>())
                , hasAlert(data.get<8>())
                , alert(data.get<9>())
                , alertType(data.get<10>())
        {
        }
        SessionRowDefinition& operator=(const SessionRowDefinition& other)
        {
            data = other.data;
            return *this;
        }
        [[nodiscard]] string table_name() const;
        [[nodiscard]] vector<ColumnType> columns() const;
        void set(const KeyStringType& key, const string& value);
        [[nodiscard]] string get(const KeyStringType& key) const;
        [[nodiscard]] string description() const;
        static vector<KeyStringType> presentableFields();
        static string presentableName();

        [[nodiscard]] Http::SessionData toSessionData() const;
        void fromSessionData(const Http::SessionData& sessionData);
        void validate(){}
};

using SessionRow = RecordImpl<SessionRowDefinition>;

} // namespace Data
