#include "ThreadPool.hpp"

namespace thread {
    
    ThreadPool::ThreadPool() 
    :   ThreadPool(std::thread::hardware_concurrency()) 
    {}

    ThreadPool::ThreadPool(int thread_count)
    :   m_threads(),
        m_tasks(),
        m_mtx(),
        m_cv(),
        m_stop_request(false)
    {
        if (thread_count <= m_minimum_thread_count) {
            thread_count = m_minimum_thread_count;
        }
        m_threads.reserve(thread_count);
        for (int i = 0; i < thread_count; ++i) {
            m_threads.emplace_back(&ThreadPool::run, this);
        }
    }

    ThreadPool::~ThreadPool() {
        m_stop_request.store(true, std::memory_order_release);
        m_cv.notify_all();
        for (auto& thread : m_threads) {
            thread.join();
        }
    }

    void ThreadPool::stop() {
        m_stop_request.store(true, std::memory_order_release);
        m_cv.notify_all();
    }

    int ThreadPool::size() const {
        return m_threads.size();
    }

    void ThreadPool::addNewThread() {
        std::lock_guard lock(m_mtx);
        m_threads.emplace_back(&ThreadPool::run, this);
    }

    void ThreadPool::run() noexcept {
        while (!m_stop_request.load(std::memory_order_acquire)) {
            std::packaged_task<void()> task;
            std::unique_lock lock(m_mtx);
            m_cv.wait(lock, [this] {
                return !m_tasks.empty() || m_stop_request.load(std::memory_order_relaxed);
            });
            if (m_stop_request.load(std::memory_order_acquire)) {
                break;
            }
            task = std::move(m_tasks.front());
            m_tasks.pop();
            lock.unlock();
            task();
        }
    }

}
