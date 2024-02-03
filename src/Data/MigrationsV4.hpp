#pragma once

#include "MigrationsV3.hpp"

namespace Data {

class MigrationsV4 : public MigrationsV3 {
public:
    void perform() override;
};

} // namespace Data
