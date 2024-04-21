#pragma once

#include "MigrationsV4.hpp"

namespace Data {

/*
 * 1) Add a column for the start_page of a user
 */

class MigrationsV5 : public MigrationsV4 {
public:
    void perform() override;
};

} // namespace Data
