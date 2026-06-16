#pragma once

#include "joint_state.hpp"
#include "timestamp.hpp"

class JointSimulator
{
public:
    JointSimulator();

    JointState generate();

private:
    float t_;
};