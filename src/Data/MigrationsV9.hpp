#pragma once
#include "MigrationsV8.hpp"
namespace Data {

/* Adds Role Column to User Table */
class MigrationsV9 : public MigrationsV8{
public:
    void perform() override;
};

} // namespace Data

