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
                             { Worker(); });
    }

    // An array of job queues
    jobQueues.resize(kJobPriorities);
    // A look-up array [0, 0, 1, 0, 1, 2, 0, 1, 2, 3]
    // The elements indicate the index of the job queue in the array
    // So that each job queue will have different priority
    // E.g. [0] repeated in the look-up array most of the time, so job queue[0] has the highest priority
    for (size_t i = 0; i < kJobPriorities; ++i) {
        for (size_t j = 0; j < i; ++j) {
            queueLookupSequence.emplace_back(j);
        }
    }

}

JobSystem::~JobSystem()
{
    // threads wol block until all threads joined
    FinishRemainTask();
}

// Non-blocking
void JobSystem::FinishRemainTask()
{
    execute = false;
    queueCV.notify_all();
}

template<typename Func>
void JobSystem::DoEnqueueJob(JobQueue &queue, JobPtr &&job, Func &&func)
{
}
