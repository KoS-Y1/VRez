#include "JobSystem.h"

#include <algorithm>

namespace
{
    constexpr int kJobPriorities = 4;
}

JobSystem::JobSystem()
{
    auto numThreads = std::max(1u, std::thread::hardware_concurrency());

    for (std::size_t i = 0; i < numThreads; ++i)
    {
        threads.emplace_back([this]
                             { Worker(); })
    }
}