#pragma once
#include "MigrationsV9.hpp"

namespace Data {

/* Adds Column or Table for Notes */
class MigrationsV10 : public MigrationsV9 {
public:
    void perform() override;
};

} // namespace Data
