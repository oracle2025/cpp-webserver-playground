#pragma once
namespace Data {

class MigrationsV0 {
public:
    virtual ~MigrationsV0() = default;
    virtual void perform();
};

} // namespace Data

