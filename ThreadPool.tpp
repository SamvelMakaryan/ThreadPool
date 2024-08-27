#ifndef THREAD_POOL_TPP_
#define THREAD_POOL_TPP_

#include "ThreadPool.hpp"

namespace thread {
 
    template <typename F, typename... Args>
    std::future<std::invoke_result_t<F, Args...>> ThreadPool::execute(F&& f, Args&&... args) {
        
        using return_type = std::invoke_result_t<F, Args...>;
        
        if (m_stop_request) {
            throw std::runtime_error("The thread pool has been used after being stopped.");
        }
        
        auto task_ptr = std::make_shared<std::packaged_task<return_type()>>(std::bind(std::forward<F>(f), std::forward<Args>(args)...));
        
        std::unique_lock lock(m_mtx);
        m_tasks.push([task_ptr]{ (*task_ptr)(); });
        lock.unlock();
        m_cv.notify_one();
        return task_ptr->get_future();
    }

}

#endif //THREAD_POOL_TPP_