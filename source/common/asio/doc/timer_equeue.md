## timer_queue_base

### op_queue 在队列上的操作

- class op_queue_access 方便访问 queue


```c++
template <typename Operation>
class op_queue
{
public:
  op_queue();

  // Destructor destroys all operations.
  ~op_queue()

  // Get the operation at the front of the queue.
  Operation* front()
  {
    return front_;
  }

  // Pop an operation from the front of the queue.
  void pop()

  // Push an operation on to the back of the queue.
  void push(Operation* h)

  template <typename OtherOperation>
  void push(op_queue<OtherOperation>& q)

  bool empty() const

  // Test whether an operation is already enqueued.
  bool is_enqueued(Operation* o) const


private:
  friend class op_queue_access;
  Operation* front_;
  Operation* back_;
};
```

### timer_queue_base
```c++
class timer_queue_base
  : private noncopyable
{
public:
  timer_queue_base() : next_(0) {}

  virtual ~timer_queue_base() {}

  virtual bool empty() const = 0;

  // Get the time to wait until the next timer.
  virtual long wait_duration_msec(long max_duration) const = 0;

  // Get the time to wait until the next timer.
  virtual long wait_duration_usec(long max_duration) const = 0;

  // Dequeue all ready timers.
  virtual void get_ready_timers(op_queue<operation>& ops) = 0;

  // Dequeue all timers.
  virtual void get_all_timers(op_queue<operation>& ops) = 0;

private:
  friend class timer_queue_set;
  //构成了单链表
  timer_queue_base* next_;
};
```
### per_timer_data

```c++
  // Per-timer data.
  class per_timer_data
  {
  private:
    friend class timer_queue;

    op_queue<wait_op> op_queue_;

    std::size_t heap_index_;

    per_timer_data* next_;
    per_timer_data* prev_;
  };
```
### timer_queue
- 定时器需要管理的资源有 per_timer_data
  - 时间间隔 time_type time_
  - 时间到期时的做动：可以有多个操作，以链表的形式管理，op_equeue
- timer_queue 以二叉堆的形式管理所有定时器，提供了对定时器的操作.

### timer_queue_set
- 所有的timer_queue 的集合，采用单链表的形式来管理timer_queue