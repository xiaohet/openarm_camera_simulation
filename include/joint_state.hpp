#pragma once

#include <vector>
#include <cstdint>

struct JointState
{
    uint64_t timestamp_ns;

    std::vector<float> position;
    std::vector<float> velocity;
    std::vector<float> torque;
};
