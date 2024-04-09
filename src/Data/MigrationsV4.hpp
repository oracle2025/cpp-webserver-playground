#pragma once

#include "MigrationsV3.hpp"

namespace Data {

/*
 * 1) Add a column for the start_page of a user
 */

class MigrationsV4 : public MigrationsV3 {
public:
    void perform() override;
};

} // namespace Data
