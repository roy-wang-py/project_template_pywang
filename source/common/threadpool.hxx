/**
 * @file threadpool.hxx
 * @author sqhao (sqhao@aibee.com)
 * @brief 
 * @version 0.1
 * @date 2019-05-30
 * 
 * @copyright Copyright (c) 2019
 * 
 */
#ifndef __SQHAO_THREADPOOL_H__
#define __SQHAO_THREADPOOL_H__

#include <vector>
#include <queue>
#include <thread>
#include <mutex>
#include <condition_variable>
#include <future>
#include <functional>
#include "default_config.hxx"

NAMESPACE_PREFIX
    /**
     * @brief ThreadPool
     * 
     */
    class ThreadPool
    {
    public:
        /**
         * @brief Construct a new Thread Pool object
         * 
         */
        ThreadPool(size_t);  

        // 添加一个任务到队列，返回任务的执行结果
        template<class F, class... Args>
        auto enqueue(F&& f, Args&&... args)   
            -> std::future<typename std::result_of<F(Args...)>::type>; 

        ~ThreadPool();
    private:
        // need to keep track of threads so we can join them
        std::vector< std::thread > workers;  
        // the task queue
        std::queue< std::function<void()> > tasks;   

        
        std::mutex queue_mutex; 
        std::condition_variable condition;   
        bool stop;
    };


    template<class F, class... Args>
    auto ThreadPool::enqueue(F&& f, Args&&... args) 
    -> std::future<typename std::result_of<F(Args...)>::type>
    {
        using return_type = typename std::result_of<F(Args...)>::type;
        auto task = std::make_shared<std::packaged_task<return_type()> >( 
            std::bind(std::forward<F>(f), std::forward<Args>(args)...));

        std::future<return_type> res = task->get_future();  
        {
            std::unique_lock<std::mutex> lock(queue_mutex); 
            if (stop)
                throw std::runtime_error("enqueue on stopped ThreadPool");

            tasks.emplace([task]() { (*task)(); }); 
        }
        condition.notify_one();
        return res;
    }
NAMESPACE_SUBFIX

#endif

