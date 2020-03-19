# operation or function
- 操作的原型
  > void （*operation）()
## scheduler_operation
- 所有操作(operation)的基类
- typedef scheduler_operation operation
- 此处的操作 operation 不是用户的 handler，应该是 asio 内部使用的一系列操作的载体

```c++
class scheduler_operation
{
public:
  typedef scheduler_operation operation_type;

  void complete(void* owner, const asio::error_code& ec,
      std::size_t bytes_transferred)
  {
    func_(owner, this, ec, bytes_transferred);
  }

  void destroy()
  {
    func_(0, this, asio::error_code(), 0);
  }

protected:
  typedef void (*func_type)(void*,
      scheduler_operation*,
      const asio::error_code&, std::size_t);

  scheduler_operation(func_type func)
    : next_(0),
      func_(func),
      task_result_(0)
  {
  }

  // Prevents deletion through this type.
  ~scheduler_operation()
  {
  }

private:
  friend class op_queue_access;
  scheduler_operation* next_; //形成单链表
  func_type func_; // operation 实例指针
protected:
  friend class scheduler;
  unsigned int task_result_; // Passed into bytes transferred. 第四个参数
};
```

## executor_op 
- 可执行的 operation

- 继承自 scheduler_operation 
- 采用 Alloc 进行 operation 的分配
- Handler 应该是用户的具体的操作请求
- 用于handler 类型 和 Alloc 的对象实例， handler中存放用户的请求,采用move copy construct
- 拥有 一个 struct  ptr，进行内存的分配和回收
- 最重要的是 静态成员函数 void do_complete (void* owner, Operation * base, error_code&, size_t)

```c++
template <typename Handler, typename Alloc,
    typename Operation = scheduler_operation>
class executor_op : public Operation
{
public:
  ASIO_DEFINE_HANDLER_ALLOCATOR_PTR(executor_op);

  template <typename H>
  executor_op(ASIO_MOVE_ARG(H) h, const Alloc& allocator)
    : Operation(&executor_op::do_complete),
      handler_(ASIO_MOVE_CAST(H)(h)),
      allocator_(allocator)
  {
  }

  static void do_complete(void* owner, Operation* base,
      const asio::error_code& /*ec*/,
      std::size_t /*bytes_transferred*/)
  {
    // 从基类转到当前子类指针
    executor_op* o(static_cast<executor_op*>(base));
    // 分配器对象
    Alloc allocator(o->allocator_);
    // 使 p.p = p.v = o
    ptr p = { detail::addressof(allocator), o, o };

    //没有实际意义
    ASIO_HANDLER_COMPLETION((*o));

    // Make a copy of the handler so that the memory can be deallocated before
    // the upcall is made. Even if we're not about to make an upcall, a
    // sub-object of the handler may be the true owner of the memory associated
    // with the handler. Consequently, a local copy of the handler is required
    // to ensure that any owning sub-object remains valid until after we have
    // deallocated the memory here.
    // handler move copy,取出 handler
    Handler handler(ASIO_MOVE_CAST(Handler)(o->handler_));
    //析构 ececutor_op
    p.reset();

    //如果有owner认领该operation,那么进行调用， Handler 的原型是 void(func)()
    if (owner)
    {
      fenced_block b(fenced_block::half);
      ASIO_HANDLER_INVOCATION_BEGIN(());
      asio_handler_invoke_helpers::invoke(handler, handler);
      ASIO_HANDLER_INVOCATION_END;
    }
  }
};

```
## executor::function

```c++

class executor::function
{
public:
  template <typename F, typename Alloc>
  explicit function(F f, const Alloc& a)
  {
    // Allocate and construct an operation to wrap the function.
    typedef detail::executor_function<F, Alloc> func_type;
    typename func_type::ptr p = {
      detail::addressof(a), func_type::ptr::allocate(a), 0 };
    // new的新用法，只构造不分配
    func_ = new (p.v) func_type(ASIO_MOVE_CAST(F)(f), a);
    p.v = 0;
  }
  // 调用存放在function中的函数
  void operator()()
  {
    if (func_)
    {
      detail::executor_function_base* func = func_;
      func_ = 0;
      func->complete();
    }
  }

private:
  // function_base 中含有 一个 func_type： void(*func_type)(function_baes*,bool)
  // base * 可以在实际调用的时候转化成特定的子类，bool =true ,被complete调用, = false ,被 destory调用
  detail::executor_function_base* func_; 
};
```

## executor_function
- 继承自 executor_function_base，类似 operation 和 scheduler_operation 的关系

## system_executor
- 在 system_executor 的 post defer 等函数调用中使用了 executor_op 
- post 的 f 包装成了 executor_op,
```c++
template <typename Function, typename Allocator>
void system_executor::post(
    ASIO_MOVE_ARG(Function) f, const Allocator& a) const
{
 // 把函数退化成函数指针，进行保存，可以在延迟调用,此处的function_type 可能是void(*)()
  typedef typename decay<Function>::type function_type;

  system_context& ctx = detail::global<system_context>();

  // Allocate and construct an operation to wrap the function.
  typedef detail::executor_op<function_type, Allocator> op;
  typename op::ptr p = { detail::addressof(a), op::ptr::allocate(a), 0 };
  p.p = new (p.v) op(ASIO_MOVE_CAST(Function)(f), a);

  ASIO_HANDLER_CREATION((ctx, *p.p,
        "system_executor", &this->context(), 0, "post"));

  ctx.scheduler_.post_immediate_completion(p.p, false);
  p.v = p.p = 0;
}
```

## executor:: impl 实现 
- 模板类 Executor 和 Allocator 是参数，有一个特化实现： 使用 system_executor
- 拥有 引用计数 能力