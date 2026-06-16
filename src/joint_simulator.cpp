#include "joint_simulator.hpp"
#include <cmath>

JointSimulator::JointSimulator() : t_(0.0f)
{
}

JointState JointSimulator::generate()
{
    JointState state;

    state.timestamp_ns = now_ns();

    for(int i=0; i<6; i++)
    {
        state.position.push_back(std::sin(t_ + i));
        state.velocity.push_back(std::cos(t_ + i));
        state.torque.push_back(0.5f * std::sin(2*t_ + i));
    }

    t_ += 0.01f;

    return state;
}