#pragma once

#include "MigrationsV1.hpp"


namespace Data {

class MigrationsV2 : public MigrationsV1 {
public:
    virtual void perform();
};

using MigrationsLatest = MigrationsV2;

} // namespace Data

