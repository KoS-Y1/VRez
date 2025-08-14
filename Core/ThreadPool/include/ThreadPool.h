#pragma once

#include <condition_variable>
#include <functional>
#include <mutex>
#include <queue>
#include <thread>
#include <vector>

#include <Singleton.h>

class ThreadPool : public Singleton<ThreadPool> {
public:
    void Enqueue(std::function<void()> &&func);
    void WaitIdle();

protected:
    ThreadPool();

    ~ThreadPool();

private:
    std::vector<std::jthread>         m_workers;
    std::queue<std::function<void()>> m_tasks;
    mutable std::mutex                m_mutex;
    std::condition_variable           m_cv;
    bool                              m_stopped = false;
    std::condition_variable           m_idleCv;
    std::atomic<size_t>               m_pendingTasks{0};

    void Worker();
    void FinishRemainWork() noexcept;
};
