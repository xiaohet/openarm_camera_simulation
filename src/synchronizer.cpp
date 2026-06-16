#include "synchronizer.hpp"
#include <cmath>

JointState Synchronizer::findNearest(uint64_t timestamp, const std::deque<JointState>& buffer)
{
    JointState best;

    uint64_t smallest_error = UINT64_MAX;

    for(const auto& sample : buffer)
    {
        uint64_t error = std::abs((long long)(sample.timestamp_ns) - (long long)(timestamp));

        if(error < smallest_error)
        {
            smallest_error = error;
            best = sample;
        }
    }

    return best;
}