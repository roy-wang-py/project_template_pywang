/**
 * @file threadpool.cpp
 * @author sqhao (sqhao@aibee.com)
 * @brief 
 * @version 0.1
 * @date 2019-05-30
 * 
 * @copyright Copyright (c) 2019
 * 
 */

#include "threadpool.hxx"
#include "log_wrapper.hxx"
NAMESPACE_PREFIX
    /**
     * @brief Construct a new Thread Pool:: Thread Pool object
     * 
     * @param threads 
     */
    ThreadPool::ThreadPool(size_t threads) : stop(false)
    {
        for (size_t i = 0; i < threads; ++i)
            workers.emplace_back([this]{
            for (;;) {
                std::function<void()> task; 
                {
                    std::unique_lock<std::mutex> lock(this->queue_mutex);
                    this->condition.wait(lock,   //直到stop 为true或者task不为空
                        [this] { return this->stop || !this->tasks.empty(); }); 
                    if (this->stop && tasks.empty())
                        return;
                    //从tasks中取出第一个任务，运行
                    task = std::move(this->tasks.front());
                    this->tasks.pop();

                }
                task(); //调用函数，运行函数
            }
        }
        );
    }

    /**
     * @brief Destroy the Thread Pool:: Thread Pool object
     *      the destructor joins all threads
     */
    ThreadPool::~ThreadPool()
    {
        {
            std::unique_lock<std::mutex> lock(queue_mutex);
            stop = true;
        }
        condition.notify_all(); 
        for (std::thread &worker : workers)
            worker.join(); 
    }

NAMESPACE_SUBFIX


