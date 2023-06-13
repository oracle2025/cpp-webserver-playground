#pragma once

#include "MigrationsV0.hpp"

namespace Data {

class MigrationsV1 : public MigrationsV0 {
public:
    void perform() override;
};

} // namespace Data
