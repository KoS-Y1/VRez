#pragma once

#include <vector>
#include <queue>
#include <thread>
#include <mutex>
#include <condition_variable>
#include <functional>

#include <Singleton.h>

class ThreadPool : public Singleton<ThreadPool>
{
public:
    void Enqueue(std::function<void()> &&func);


protected:
        ThreadPool();

    ~ThreadPool();

private:
    void Worker();
    void FinishRemainWork() noexcept;

    std::vector<std::jthread> m_workers;
    std::queue<std::function<void()>> m_tasks;
    mutable std::mutex m_mutex;
    std::condition_variable  m_cv;
    bool m_stopped = false;
};
