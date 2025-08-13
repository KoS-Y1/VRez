#include "include/ThreadPool.h"

#include <Debug.h>

ThreadPool::ThreadPool() {
    size_t threadCount = std::thread::hardware_concurrency();

    if (threadCount == 0) {
        threadCount = 1;
    }

    m_workers.reserve(threadCount);

    for (size_t i = 0; i < threadCount; ++i) {
        m_workers.emplace_back([this]() { Worker(); });
    }
}

ThreadPool::~ThreadPool() {
    FinishRemainWork();
}

void ThreadPool::Enqueue(std::function<void()> &&func) {
    {
        std::scoped_lock lock(m_mutex);
        DEBUG_ASSERT(m_stopped == false);
        m_tasks.push(std::move(func));
    }
    m_cv.notify_one();
}

void ThreadPool::Worker() {
    while (true) {
        std::function<void()> job;
        {
            std::unique_lock lock(m_mutex);
            // Wait for an available task
            m_cv.wait(lock, [this]() { return !m_tasks.empty() || m_stopped; });

            // Exit when
            // 1) Thread pool is shutting down and all tasks are finished
            // 2) Explicitly asked to stop
            if (m_stopped && m_tasks.empty()) {
                return;
            }

            // Fetch one task
            job = std::move(m_tasks.front());
            m_tasks.pop();
        }

        job();
    }
}

void ThreadPool::FinishRemainWork() noexcept {
    {
        std::scoped_lock lock(m_mutex);
        // Making sure it only called once
        if (m_stopped) {
            return;
        }
        m_stopped = true;
    }
    m_cv.notify_all();

    m_workers.clear();
}
