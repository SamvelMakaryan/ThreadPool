#ifndef THREAD_POOL_HPP_
#define THREAD_POOL_HPP_

#include <condition_variable>
#include <type_traits>
#include <functional>
#include <stdexcept>
#include <concepts>
#include <utility>
#include <vector>
#include <thread>
#include <atomic>
#include <future>
#include <mutex>
#include <queue>

namespace thread {

    class ThreadPool {
        public:    
            ThreadPool();
            explicit ThreadPool(int);
            ThreadPool(const ThreadPool&) = delete;
            ThreadPool(ThreadPool&&) noexcept = delete;
            ThreadPool &operator=(const ThreadPool&) = delete;
            ThreadPool &operator=(ThreadPool&&) noexcept = delete;
            ~ThreadPool();
        public:
            void stop();
            int size() const;
            void addNewThread();

            template <typename F, typename... Args, typename return_type = std::invoke_result_t<F, Args...>>
            std::future<return_type> execute(F&&, Args&&...) requires std::invocable<F, Args...>;
        private:
            void run() noexcept;
        private:
            constinit const static int m_minimum_thread_count = 2;
            std::vector<std::thread> m_threads;
            std::queue<std::packaged_task<void()>> m_tasks;
            std::mutex m_mtx;
            std::condition_variable m_cv;
            std::atomic<bool> m_stop_request;
    };

}

#include "ThreadPool.tpp"

#endif //THREAD_POOL_HPP_