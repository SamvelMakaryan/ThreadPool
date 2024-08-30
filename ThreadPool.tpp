#ifndef THREAD_POOL_TPP_
#define THREAD_POOL_TPP_

#include "ThreadPool.hpp"

namespace thread {
 
    template <typename F, typename... Args, typename return_type = std::invoke_result_t<F, Args...>>
    std::future<return_type> ThreadPool::execute(F&& f, Args&&... args) {
        if (m_stop_request) {
            throw std::runtime_error("The thread pool has been used after being stopped.");
        }
        
        std::packaged_task<return_type(Args...)> task(std::forward<F>(f));
        std::future<return_type> future = task.get_future();
        std::packaged_task<void()> wrapper(std::move([task = std::move(task), ... args = std::forward<Args>(args)]() mutable {
                                                        task(std::forward<Args>(args)...);
                                                    }));
        {
            std::lock_guard lock(m_mtx);
            m_tasks.push(std::move(wrapper));
        }

        m_cv.notify_one();
        return future;
    }

}

#endif //THREAD_POOL_TPP_