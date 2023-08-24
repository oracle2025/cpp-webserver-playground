#pragma once

#include "MigrationsV1.hpp"


namespace Data {

class MigrationsV2 : public MigrationsV1 {
public:
    void perform() override;
};


} // namespace Data

