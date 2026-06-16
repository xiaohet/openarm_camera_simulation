#include "timestamp.hpp"

uint64_t now_ns()
{
    return std::chrono::duration_cast<std::chrono::nanoseconds>
        (std::chrono::steady_clock::now().time_since_epoch()).count();
}