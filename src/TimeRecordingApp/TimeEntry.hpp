#pragma once
#include "Data/RecordImpl.hpp"

#include <Poco/Tuple.h>

struct TimeEntryDefinition {
    using string = std::string;
    using RecordType = Poco::Tuple<
        string,
        string,
        string,
        string,
        string,
        string,
        string,
        string,
        string>;
    RecordType data;
    string& id;
    string& employee_id;
    string& event_date;
    string& event_time;
    string& event_type;
    string& corrected_event_id;
    string& deleted_event_id;
    string& creation_date;
    string& creator_user_id;
    explicit TimeEntryDefinition(RecordType d)
        : data{std::move(d)}
        , id(data.get<0>())
        , employee_id(data.get<1>())
        , event_date(data.get<2>())
        , event_time(data.get<3>())
        , event_type(data.get<4>())
        , corrected_event_id(data.get<5>())
        , deleted_event_id(data.get<6>())
        , creation_date(data.get<7>())
        , creator_user_id(data.get<8>())
    {
    }
        TimeEntryDefinition()
                : data{"", "", "", "", "", "", "", "", ""}
                , id(data.get<0>())
                , employee_id(data.get<1>())
                , event_date(data.get<2>())
                , event_time(data.get<3>())
                , event_type(data.get<4>())
                , corrected_event_id(data.get<5>())
                , deleted_event_id(data.get<6>())
                , creation_date(data.get<7>())
                , creator_user_id(data.get<8>())
        {
        }
        TimeEntryDefinition(const TimeEntryDefinition& t)
                : data{t.data}
                , id(data.get<0>())
                , employee_id(data.get<1>())
                , event_date(data.get<2>())
                , event_time(data.get<3>())
                , event_type(data.get<4>())
                , corrected_event_id(data.get<5>())
                , deleted_event_id(data.get<6>())
                , creation_date(data.get<7>())
                , creator_user_id(data.get<8>())
        {
        }
        TimeEntryDefinition& operator=(const TimeEntryDefinition& other)
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
};

using TimeEntry = RecordImpl<TimeEntryDefinition>;