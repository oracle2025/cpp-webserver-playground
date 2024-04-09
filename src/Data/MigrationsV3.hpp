#pragma once

#include "MigrationsV2.hpp"

namespace Data {

/*
 * 1) Convert the column password from VARCHAR to BLOB
 * 2) Hash the existing passwords
 */

class MigrationsV3 : public MigrationsV2{
public:
    void perform() override;
};


} // namespace Data

