#pragma once

#include "Job.h"
#include <Singleton.h>

#include <condition_variable>
#include <deque>
#include <thread>
#include <vector>

// Only one JobSystem instance to manage all jobs
class JobSystem : public Singleton<JobSystem>
{
public:
    template <class Job, class... Args>
    void Schedule(Args &&...args)
    {
        // Use forward to preserve arg type
        EnqueueJob(std::make_unique<Job>(std::forward<Args>(args)...));
    }

    template <class Job, class... Args>
    void ScheduleImportant(Args &&...args)
    {
        // Use forward to preserve arg type
        EnqueueJobImportant(std::make_unique<Job>(std::forward<Args>(args)...));
    }

protected:
    JobSystem();
    ~JobSystem();

private:
    using JobPtr = std::unique_ptr<Job>;

    // Why not just vector<deque<unique_ptr<Job>>>
    // Since MSVC/C++ standard has an unbelievable bug:
    // https://www.reddit.com/r/cpp/comments/6q94ai/comment/dkwdd8l/
    class JobQueue : public std::deque<JobPtr>
    {
    public:
        JobQueue() = default;
        // Use deque move for move constructor
        JobQueue(JobQueue &&queue) noexcept : std::deque<JobPtr>(std::move(queue)) {};
        // Disallow copy constructor
        JobQueue(JobQueue const &) = delete;
    };

    template <typename Func>
    void DoEnqueueJob(JobQueue &queue, JobPtr &&job, Func &&func);

    void EnqueueJob(JobPtr &&job, std::size_t priority = 0);

    void EnqueueJobImportant(JobPtr &&job);

    bool AllQueueEmpty() const;

    void FinishRemainTask();

    void Worker();

    JobPtr FetchOne(std::size_t &lookup_idx);

    // Flag to indicate if to execute the thread pool
    std::atomic<bool> execute{true};
    std::vector<JobQueue> jobQueues;
    std::condition_variable queueCV;
    std::mutex queueMutex;
    std::vector<std::size_t> queueLookupSequence;
    std::vector<std::jthread> threads;
};
