#pragma once

#include "RecordImpl.hpp"

#include <Poco/Tuple.h>

namespace Data {

struct MovieDefinition {
        using RecordType = Poco::Tuple<std::string, std::string, std::string, std::string, std::string, std::string, std::string>;
        RecordType data;
        std::string& id;
        std::string& title;
        std::string& director;
        std::string& genre;
        std::string& year;
        std::string& rating;
        std::string& user_id;
        explicit MovieDefinition(RecordType d);
        MovieDefinition();
        MovieDefinition(const MovieDefinition& t);
        std::string description() const;
        std::string get(const KeyStringType& key) const;
        void set(const KeyStringType& key, const std::string& value);
        static std::vector<ColumnType> columns();
        static std::string table_name();
        static std::vector<KeyStringType> presentableFields();
        static std::string presentableName();
};

using Movie = RecordImpl<MovieDefinition>;

} // namespace Data

