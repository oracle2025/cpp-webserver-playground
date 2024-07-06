#pragma once
#include "MigrationsV7.hpp"

namespace Data {

/* Adds Table TimeEntry */
class MigrationsV8 : public MigrationsV7 {
public:
    void perform() override;
};

} // namespace Data
