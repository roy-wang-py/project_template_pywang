# schedulaer

- scheduler_thread_info 存放当前调度线程的信息，包括未完成任务的数量和该线程发现的任务队列

```c++
class posix_event{
private:
  ::pthread_cond_t cond_;  //线程条件变量，静态初始化
  std::size_t state_; //状态机
  //如果 state_ bit 0 为0,说明还没有达到唤醒的条件
  //如果 state_ bit 0 为1,说明唤醒了在此事件上的线程
  // 剩下的31位可以来表示等待线程数量
};
```

- posix_event 内部拥有一个pthread_cond_t 变量，只要采用该event的函数，那么就将阻塞在该事件上或者被该事件唤醒
- signal_all() 、signal() 唤醒该事件上的所有线程
- unlock_and_signal_one() 解锁并且如果有线程阻塞在该事件上，那么唤醒一个线程
- maybe_unlock_and_signal_one() 如果有线程阻塞在该事件上，那么解锁并唤醒一个线程
- wait() 等待条件变成变成可唤醒的
- wait_for_usec() 最多等待timeout就返回

- 内部没有多线程，但是支持多线程，每一个运行run的线程，都会认为是在一个线程集合中
- 如果不需要多线程并行，那么就不必要加锁，所以用conditionally_enabled_event 统一处理
- thread_function 运行调度器的run接口，thread_线程可以驱动run运行
- task 是指 epoll_reactor，在全局中只有一个epoll_reactor实例
- work 是指用户的操作
- task_cleanup 的作用是析构函数中 把线程的 private_op_queue和task_operation_占位符放入公共的op_queue中
- init_task()
  >mutex lock --> task 关联epoll_reactor -->task_operation_ 占位符放入op_queue -->如果只有一个线程在阻塞，那么唤醒一个线程
-  do_run_one()
  > 如果op_queue是空队列，那么wait

- post_deferred_completions() 发送一个延迟完成队列操作



```c++
class conditionally_enabled_event
  : private noncopyable
{

private:
  asio::detail::event event_;
};
```

```c++
class scheduler
  : public execution_context_service_base<scheduler>,
    public thread_context
{
public:
  typedef scheduler_operation operation;

  // Constructor. Specifies the number of concurrent threads that are likely to
  // run the scheduler. If set to 1 certain optimisation are performed.
  ASIO_DECL scheduler(asio::execution_context& ctx,
      int concurrency_hint = 0, bool own_thread = true);

  // Destructor.
  ASIO_DECL ~scheduler();

  // Destroy all user-defined handler objects owned by the service.
  ASIO_DECL void shutdown();

  // Initialise the task, if required.
  ASIO_DECL void init_task();

  // Run the event loop until interrupted or no more work.
  ASIO_DECL std::size_t run(asio::error_code& ec);

  // Run until interrupted or one operation is performed.
  ASIO_DECL std::size_t run_one(asio::error_code& ec);

  // Run until timeout, interrupted, or one operation is performed.
  ASIO_DECL std::size_t wait_one(
      long usec, asio::error_code& ec);

  // Poll for operations without blocking.
  ASIO_DECL std::size_t poll(asio::error_code& ec);

  // Poll for one operation without blocking.
  ASIO_DECL std::size_t poll_one(asio::error_code& ec);

  // Interrupt the event processing loop.
  ASIO_DECL void stop();

  // Determine whether the scheduler is stopped.
  ASIO_DECL bool stopped() const;

  // Restart in preparation for a subsequent run invocation.
  ASIO_DECL void restart();

  // Notify that some work has started.
  void work_started()
  {
    ++outstanding_work_;
  }

  // Used to compensate for a forthcoming work_finished call. Must be called
  // from within a scheduler-owned thread.
  ASIO_DECL void compensating_work_started();

  // Notify that some work has finished.
  void work_finished()
  {
    if (--outstanding_work_ == 0)
      stop();
  }

  // Return whether a handler can be dispatched immediately.
  bool can_dispatch()
  {
    return thread_call_stack::contains(this) != 0;
  }

  // Request invocation of the given operation and return immediately. Assumes
  // that work_started() has not yet been called for the operation.
  ASIO_DECL void post_immediate_completion(
      operation* op, bool is_continuation);

  // Request invocation of the given operation and return immediately. Assumes
  // that work_started() was previously called for the operation.
  ASIO_DECL void post_deferred_completion(operation* op);

  // Request invocation of the given operations and return immediately. Assumes
  // that work_started() was previously called for each operation.
  ASIO_DECL void post_deferred_completions(op_queue<operation>& ops);

  // Enqueue the given operation following a failed attempt to dispatch the
  // operation for immediate invocation.
  ASIO_DECL void do_dispatch(operation* op);

  // Process unfinished operations as part of a shutdownoperation. Assumes that
  // work_started() was previously called for the operations.
  ASIO_DECL void abandon_operations(op_queue<operation>& ops);

  // Get the concurrency hint that was used to initialise the scheduler.
  int concurrency_hint() const
  {
    return concurrency_hint_;
  }

private:
  // The mutex type used by this scheduler.
  typedef conditionally_enabled_mutex mutex;

  // The event type used by this scheduler.
  typedef conditionally_enabled_event event;

  // Structure containing thread-specific data.
  typedef scheduler_thread_info thread_info;

  // Run at most one operation. May block.
  ASIO_DECL std::size_t do_run_one(mutex::scoped_lock& lock,
      thread_info& this_thread, const asio::error_code& ec);

  // Run at most one operation with a timeout. May block.
  ASIO_DECL std::size_t do_wait_one(mutex::scoped_lock& lock,
      thread_info& this_thread, long usec, const asio::error_code& ec);

  // Poll for at most one operation.
  ASIO_DECL std::size_t do_poll_one(mutex::scoped_lock& lock,
      thread_info& this_thread, const asio::error_code& ec);

  ASIO_DECL void stop_all_threads(mutex::scoped_lock& lock);

  // Wake a single idle thread, or the task, and always unlock the mutex.
  ASIO_DECL void wake_one_thread_and_unlock(
      mutex::scoped_lock& lock);

  // Helper class to run the scheduler in its own thread.
  class thread_function;
  friend class thread_function;

  // Helper class to perform task-related operations on block exit.
  struct task_cleanup;
  friend struct task_cleanup;

  // Helper class to call work-related operations on block exit.
  struct work_cleanup;
  friend struct work_cleanup;

  // Whether to optimise for single-threaded use cases.
  const bool one_thread_;

  // Mutex to protect access to internal data.
  mutable mutex mutex_;

  // Event to wake up blocked threads.
  event wakeup_event_;

  // The task to be run by this service.
  //typedef class epoll_reactor reactor;
  reactor* task_;

  // 占位符，表示在队列中的当前任务的位置
  struct task_operation : operation
  {
    task_operation() : operation(0) {}
  } task_operation_;

  // Whether the task has been interrupted.
  bool task_interrupted_;

  // 未完成任务的数量
  atomic_count outstanding_work_;

  // 准备好被分发的处理队列
  op_queue<operation> op_queue_;

  // Flag to indicate that the dispatcher has been stopped.
  bool stopped_;

  // Flag to indicate that the dispatcher has been shut down.
  bool shutdown_;

  // The concurrency hint used to initialise the scheduler.
  const int concurrency_hint_;

  // 运行调度器的线程,构造的时候创建
  asio::detail::thread* thread_;
};
```

```c++
std::size_t scheduler::do_run_one(mutex::scoped_lock& lock,
    scheduler::thread_info& this_thread,
    const asio::error_code& ec)
{

      // Prepare to execute first handler from queue.
      operation* o = op_queue_.front();
      op_queue_.pop();
      bool more_handlers = (!op_queue_.empty());

      if (o == &task_operation_)
      {
        task_interrupted_ = more_handlers;

        if (more_handlers && !one_thread_)
          wakeup_event_.unlock_and_signal_one(lock);
        else
          lock.unlock();

        task_cleanup on_exit = { this, &lock, &this_thread };
        (void)on_exit;

        // Run the task. May throw an exception. Only block if the operation
        // queue is empty and we're not polling, otherwise we want to return
        // as soon as possible.
        task_->run(more_handlers ? 0 : -1, this_thread.private_op_queue);
      }

}
```