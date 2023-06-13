#pragma once

namespace Data {

class Migration {
public:
    Migration();
    int version();
    bool hasVersion();
    void version(int version);
    bool isDatabaseAtVersion(int version);
};

} // namespace Data
