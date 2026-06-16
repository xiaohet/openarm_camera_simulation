#pragma once

#include <deque>

#include "joint_state.hpp"

class Synchronizer
{
public:

    static JointState findNearest(uint64_t timestamp, const std::deque<JointState>& buffer);
};