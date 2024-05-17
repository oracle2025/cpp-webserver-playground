#pragma once
#include "MigrationsV5.hpp"
namespace Data {

/* Adds Table Movie */
class MigrationsV6 : public MigrationsV5 {
public:
    void perform() override;
};

} // namespace Data
