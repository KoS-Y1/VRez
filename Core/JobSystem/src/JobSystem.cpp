#include "include/JobSystem.h"

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
        threads.emplace_back([this] { Worker(); });
    }

    // An array of job queues
    jobQueues.resize(kJobPriorities);
    // A look-up array [0, 0, 1, 0, 1, 2, 0, 1, 2, 3]
    // The elements indicate the index of the job queue in the array
    // So that each job queue will have different priority
    // E.g. [0] repeated in the look-up array most of the time, so job queue[0] has the highest priority
    for (size_t i = 0; i < kJobPriorities; ++i)
    {
        for (size_t j = 0; j < i; ++j)
        {
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

// func is a function that decides how the job enqueued
// Func&& forwarding technique
template<typename Func>
void JobSystem::DoEnqueueJob(JobQueue &queue, JobPtr &&job, Func &&func)
{ {
        std::lock_guard lock(queueMutex);
        job->jobSystem = this;
        // func must handle a movable job, not a copy
        func(queue, std::move(job));
    }
    queueCV.notify_all();
}

void JobSystem::EnqueueJob(JobPtr &&job, std::size_t priority)
{
    priority = std::clamp(priority, 0ull, jobQueues.size());
    DoEnqueueJob(jobQueues[priority], std::move(job), [](JobQueue &queue, JobPtr &&job)
    {
        queue.push_back(std::move(job));
    });
}

void JobSystem::EnqueueJobImportant(JobPtr &&job)
{
    DoEnqueueJob(jobQueues.front(), std::move(job), [](JobQueue &queue, JobPtr &&job)
    {
        queue.push_back(std::move(job));
    });
}

bool JobSystem::AllQueueEmpty() const
{
    return std::ranges::all_of(jobQueues, [](const auto &queue)
    {
        return queue.empty();
    });
}

JobSystem::JobPtr JobSystem::FetchOne(std::size_t &lookupIdx)
{
    std::unique_lock lock(queueMutex);
    // Wait until one of the queue is not empty or job system is not executing
    queueCV.wait(lock, [&]()
    {
        return !AllQueueEmpty() || !execute;
    });
    // Discard remaining tasks
    if (!execute)
    {
        return nullptr;
    }
    while (true)
    {
        // Get the queue
        auto &queue = jobQueues[queueLookupSequence[lookupIdx]];
        if (!queue.empty())
        {
            // Pop out the front job in the queue
            auto job = std::move(queue.front());
            queue.pop_front();
            return job;
        }
        // increase the lookupIdx
        lookupIdx = (lookupIdx + 1) % queueLookupSequence.size();
    }
}

void JobSystem::Worker()
{
    std::size_t lookupIdx = 0;
    while (true)
    {
        auto job = FetchOne(lookupIdx);
        if (!job)
        {
            return;
        }

        if (!job->ReadyForExecution())
        {
            // Get previous queue
            const std::size_t prevIdx = (lookupIdx + queueLookupSequence.size() - 1) % queueLookupSequence.size();
            const size_t prevQueuePriority = queueLookupSequence[prevIdx];
            const size_t lowerPriority = std::min(prevQueuePriority + 1, jobQueues.size() - 1);
            EnqueueJob(std::move(job), lowerPriority);
            continue;
        }

        // Check reschedule policy
        switch (job->Execute())
        {
            case Job::CompletionPolicy::Complete:
                // Do nothing
                break;
            case Job::CompletionPolicy::Reschedule:
                EnqueueJob(std::move(job), 0);
                break;
            case Job::CompletionPolicy::RescheduleImportant:
                EnqueueJobImportant(std::move(job));
                break;
            default:
                break;
        }
    }
}
