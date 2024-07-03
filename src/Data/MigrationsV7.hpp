#pragma once
#include "MigrationsV6.hpp"

namespace Data {

/* Adds Table Session */
class MigrationsV7 :public MigrationsV6{
public:
    void perform() override;
};

} // namespace Data

