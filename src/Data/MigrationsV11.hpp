#pragma once
#include "MigrationsV10.hpp"
namespace Data {

/* Adds Table for Calendar Categories and Calendar Entries */
class MigrationsV11 : public MigrationsV10 {
public:
    void perform() override;
};

} // namespace Data
