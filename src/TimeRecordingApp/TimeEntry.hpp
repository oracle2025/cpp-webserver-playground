#pragma once
#include "Data/RecordImpl.hpp"

#include <Poco/Data/Date.h>
#include <Poco/Tuple.h>

struct TimeEntryDefinition {
    using string = std::string;
    using RecordType = Poco::Tuple<
        string,
        string,
        Poco::Data::Date,
        Poco::Data::Time,
        string,
        string,
        string,
        string,
        string,
        string>;
    RecordType data;
    string& id;
    string& employee_id;
    Poco::Data::Date& event_date;
    Poco::Data::Time& event_time;
    string& event_type;
    string& corrected_event_id;
    string& deleted_event_id;
    string& creation_date;
    string& creator_user_id;
    string& note;
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
        , note(data.get<9>())
    {
    }
    TimeEntryDefinition()
        : data{"", "", {}, {}, "", "", "", "", ""}
        , id(data.get<0>())
        , employee_id(data.get<1>())
        , event_date(data.get<2>())
        , event_time(data.get<3>())
        , event_type(data.get<4>())
        , corrected_event_id(data.get<5>())
        , deleted_event_id(data.get<6>())
        , creation_date(data.get<7>())
        , creator_user_id(data.get<8>())
        , note(data.get<9>())
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
        , note(data.get<9>())
    {
    }
    TimeEntryDefinition& operator=(const TimeEntryDefinition& other)
    {
        data = other.data;
        return *this;
    }
    [[nodiscard]] static string table_name();
    [[nodiscard]] static vector<ColumnType> columns();
    void set(const KeyStringType& key, const string& value);
    [[nodiscard]] string get(const KeyStringType& key) const;
    [[nodiscard]] string description() const;
    static vector<KeyStringType> presentableFields();
    static string presentableName();
    [[nodiscard]] static bool isEmptyFor(const string& user_id);
    [[nodiscard]] static std::vector<int> yearsFor(const string& user_id);
    [[nodiscard]] std::vector<int> yearsForAllUsers() const;
    [[nodiscard]] std::vector<int> monthsFor(
        const string& user_id, int year) const;
    [[nodiscard]] std::vector<int> monthsForAllUsers(int year) const;
    vector<shared_ptr<Record>> overviewAsPointers(
        const string& user_id, int year, int month, const string& current_date);
    vector<shared_ptr<Record>> listDay(
        const string& user_id, const string& current_date);
    struct IsOpenResult {
        bool isOpen;
        string start_time;
        string note;
    };
    IsOpenResult isOpen(const string& user_id, const string& date);

    void closeOpenDays(const string& user_id, const string& current_date);
    [[nodiscard]] Poco::Data::Date getEventDate() const;
    bool checkTimestampExists(
        const string& employee_id,
        const string& event_date,
        const string& event_time);
    bool checkTimerangeOverlaps(
        const string& employee_id,
        const string& event_date,
        const string& start_time,
        const string& end_time);
    void validate();
};

using TimeEntry = RecordImpl<TimeEntryDefinition>;