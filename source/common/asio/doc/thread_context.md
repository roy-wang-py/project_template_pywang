# thread_context

## thread_info_base
- 对线程中内存的一些分配和回收

```c++
static void* allocate(thread_info_base* this_thread, std::size_t size /* 需要分配的字节数 */)
  { // 注意这里chunk没有明显的语义，它就是个最小存储单位，即4字节
    // 由字节数得到需要分配的chunk数，注意向上取整
    std::size_t chunks = (size + chunk_size - 1) / chunk_size;  

    if (this_thread && this_thread->reusable_memory_)//判断传进来的this_thread是不是空指针且它内部所指的那块可重用内存空间是不是空的
    { // 如果有现有的可重用内存空间，就先判断现有的内存空间够不够大，如果足够塞下那就直接返回，不够的话就重新申请内存
      void* const pointer = this_thread->reusable_memory_; // 把这块我们需要操作的内存空间暂存下来
      this_thread->reusable_memory_ = 0;  // 先把它置空晾一边

      unsigned char* const mem = static_cast<unsigned char*>(pointer);  // 转成char数组方便指针运算
      //这里是个小优化，后面讲。只要知道mem[0]内存的是这块内存的chunk数目大小就行了
      if (static_cast<std::size_t>(mem[0]) >= chunks)  // 如果这块内存空间的chunk足够多，那就直接返回
      {
      	// 把chunk数目大小存在这块内存空间的最后面
      	// （注意这里不会内存越界，因为实际上总会比记录的chunk多分配一个字节）
      	// （还有人会问一个字节就够存吗？再回到上面看一下，我们实际上只用了一个字节来表示chunk数量，mem[0]取到的只有一个字节，毕竟它只是char，不是int）
        mem[size] = mem[0];
        return pointer;
      }
		// 否则若这块内存不够我们想要分配的量，那就删掉这块内存重新分配
      ::operator delete(pointer);
    }
	//以下是重新分配内存逻辑
    void* const pointer = ::operator new(chunks * chunk_size + 1); // 注意此处多分配了一个字节
    unsigned char* const mem = static_cast<unsigned char*>(pointer);
    // UCHAR_MAX代表'unsigned char'可以保存的最大值（因为我们的chunk数相当于是用一个unsigned char来保存的呀）
    mem[size] = (chunks <= UCHAR_MAX) ? static_cast<unsigned char>(chunks) : 0;  // 把chunk数存到尾端
    return pointer;
  }

  static void deallocate(thread_info_base* this_thread,
      void* pointer, std::size_t size)  // pointer所指的内存才是要释放的内存
  {
    if (size <= chunk_size * UCHAR_MAX)  // 这块内存的chunk数是不是超出了一个字节所能表示的极限
    {
      if (this_thread && this_thread->reusable_memory_ == 0)
      {
        unsigned char* const mem = static_cast<unsigned char*>(pointer);
        mem[0] = mem[size];  // 把存在尾端的chunk数重新存到第一个字节
        this_thread->reusable_memory_ = pointer;
        return;
      }
    }
    ::operator delete(pointer);
  }

```

## 自动传入 thread_info_base
handler_alloc_hook.hpp/ipp
- 对于句柄对象默认的分配/释放函数，一般地，句柄对象都是小内存对象
- 异步操作可能需要分配零时对象，因为异步操作都有一个句柄函数对象，这些临时对象需要关联句柄

```c++
void* asio_handler_allocate(std::size_t size, ...)
{
  return detail::thread_info_base::allocate(
      detail::thread_context::thread_call_stack::top(), size);
}

void asio_handler_deallocate(void* pointer, std::size_t size, ...)
{
  detail::thread_info_base::deallocate(
      detail::thread_context::thread_call_stack::top(), pointer, size);

}
```
## asio_handler_alloc_helpers
- handler_alloc_helpers.hpp
  
```c++
template <typename Handler>
inline void* allocate(std::size_t s, Handler& h)
{
  using asio::asio_handler_allocate;
  // addressof(h) 返回所引用的对象的实际地址,实际没有使用addressof()
  return asio_handler_allocate(s, asio::detail::addressof(h));

}

template <typename Handler>
inline void deallocate(void* p, std::size_t s, Handler& h)
{
  using asio::asio_handler_deallocate;
  asio_handler_deallocate(p, s, asio::detail::addressof(h));
}

```

## hook_allocator

```c++
template <typename Handler, typename T>
class hook_allocator
{
public:
  explicit hook_allocator(Handler& h)
  template <typename U>
  hook_allocator(const hook_allocator<Handler, U>& a)

  T* allocate(std::size_t n)

  void deallocate(T* p, std::size_t n)

//private:
  Handler& handler_;
};
```
- 模板特化
```c++
template <typename Handler>
class hook_allocator<Handler, void>
```

## recycling_allocator 
- 返回 allocator 对象的实例，实际上使用模板技术普通数据结构和自定义的数据结构统一,对外提供统一的分配器

- recycling_allocator 是对普通数据结构的封装。
- allocator(n) 返回n个T 对象的首地址
```c++
template <typename T, typename Purpose = thread_info_base::default_tag>
class recycling_allocator
{
public:
  typedef T value_type;

  template <typename U>
  struct rebind
  {
    typedef recycling_allocator<U, Purpose> other;
  };

  recycling_allocator()
  {
  }

  template <typename U>
  recycling_allocator(const recycling_allocator<U, Purpose>&)
  {
  }

  T* allocate(std::size_t n)
  {
    //重复利用当前线程的内存，提高效率
    typedef thread_context::thread_call_stack call_stack;
    void* p = thread_info_base::allocate(Purpose(),
        call_stack::top(), sizeof(T) * n);
    return static_cast<T*>(p);
  }

  void deallocate(T* p, std::size_t n)
  {
    typedef thread_context::thread_call_stack call_stack;
    thread_info_base::deallocate(Purpose(),
        call_stack::top(), p, sizeof(T) * n);
  }
};

```
- 有分配器的，get函数直接返回该分配器的object
```c++
template <typename Allocator, typename Purpose>
struct get_recycling_allocator
{
  typedef Allocator type;
  static type get(const Allocator& a) { return a; }
};
```
- 普通的数据结构类，使用std默认的分配器包装一下，对外提供一个特定数据结构的分配器
```c++
template <typename T, typename Purpose>
struct get_recycling_allocator<std::allocator<T>, Purpose>
{
  typedef recycling_allocator<T, Purpose> type;
  static type get(const std::allocator<T>&) { return type(); }
};
```


## ASIO_DEFINE_HANDLER_PTR


## ASIO_DEFINE_TAGGED_HANDLER_ALLOCATOR_PTR
- 宏定义分配器指针,purpose 是 tag，没有实际用处，op 是操作函数句柄类
```c++
#define ASIO_DEFINE_TAGGED_HANDLER_ALLOCATOR_PTR(purpose, op) 
  struct ptr 
  { 
    const Alloc* a;  // Alloc 是模板参数之一，分配器
    void* v;  // 如果v 不为null，那么v和p 指向同一个对象,v用于delete 内存
    op* p;  // handler类指针,用于析构
    ~ptr()  //析构函数 
    { 
      reset(); 
    } 

    // 静态函数，使用分配器a 分配 op 类对象
    static op* allocate(const Alloc& a) 
    { 
      //typename ::asio::detail::get_recycling_allocator<Alloc, purpose>::type 是包装好的分配器类型，别名叫 recycling_allocator_type
      typedef typename ::asio::detail::get_recycling_allocator< 
        Alloc, purpose>::type recycling_allocator_type; 
      // 获取 分配器对象a1
      ASIO_REBIND_ALLOC(recycling_allocator_type, op) a1(      ::asio::detail::get_recycling_allocator<Alloc, purpose>::get(a)); 
      // 利用分配器分配1个op
      return a1.allocate(1); 
    } 
    void reset() 
    { 
      if (p)  // 如果p 分配了内存，那么主动调用 该对象的析构函数进行对象的析构
      { 
        p->~op(); 
        p = 0; 
      } 
      if (v) //如果v 分配了内存，delete
      { 
        typedef typename ::asio::detail::get_recycling_allocator< 
          Alloc, purpose>::type recycling_allocator_type; 
        ASIO_REBIND_ALLOC(recycling_allocator_type, op) a1( 
              ::asio::detail::get_recycling_allocator< 
                Alloc, purpose>::get(*a)); 
        a1.deallocate(static_cast<op*>(v), 1); 
        v = 0; 
      } 
    } 
  } 

```

## call_stack<thread_context,thread_info_base> thread_call_statck

- detail::thread_context::thread_call_stack::top() 返回线程栈中私有数据的栈顶context,该context中存放有thread_info_base* value 数据


## executor_function

### executor_function_base
- 可执行函数对象基类接口
```c++
class executor_function_base
{
public:
  void complete()
  {
    func_(this, true);
  }

  void destroy()
  {
    func_(this, false);
  }

protected:
  typedef void (*func_type)(executor_function_base*, bool);

  executor_function_base(func_type func)
    : func_(func)
  {
  }
  // Prevents deletion through this type.
  ~executor_function_base()
  {
  }

private:
  func_type func_;
};
```
### executor_function
- 派生于 executor_function_base
- 定义了函数对象类，创建的executor_function对象，会把do_complete传入基类中
- 在do_complete中
  >从executor_function 中提取出function_-->使用p.reset()delete 派生类的内存，只留下基类，调用function or not 后，基类进行析构
```c++
template <typename Function, typename Alloc>
class executor_function : public executor_function_base
{
public:
  ASIO_DEFINE_TAGGED_HANDLER_ALLOCATOR_PTR(
      thread_info_base::executor_function_tag, executor_function);

  template <typename F>
  executor_function(ASIO_MOVE_ARG(F) f, const Alloc& allocator)
    : executor_function_base(&executor_function::do_complete),
      function_(ASIO_MOVE_CAST(F)(f)),
      allocator_(allocator)
  {
  }

  static void do_complete(executor_function_base* base, bool call)
  {
    // Take ownership of the function object.
    executor_function* o(static_cast<executor_function*>(base));
    Alloc allocator(o->allocator_);
    ptr p = { detail::addressof(allocator), o, o };

    // Make a copy of the function so that the memory can be deallocated before
    // the upcall is made. Even if we're not about to make an upcall, a
    // sub-object of the function may be the true owner of the memory
    // associated with the function. Consequently, a local copy of the function
    // is required to ensure that any owning sub-object remains valid until
    // after we have deallocated the memory here.
    Function function(ASIO_MOVE_CAST(Function)(o->function_));
    p.reset();

    // Make the upcall if required.
    if (call)
    {
      function();
    }
  }

private:
  Function function_; // 函数对象类
  Alloc allocator_; //分配器
};
```
## executor
- executors的多态包装器

### impl_base
- 对所有的多态executor实现类的虚基类
```c++
class impl_base
  {
  public:
    virtual impl_base* clone() const ASIO_NOEXCEPT = 0;
    virtual void destroy() ASIO_NOEXCEPT = 0;
    virtual execution_context& context() ASIO_NOEXCEPT = 0; // 可执行上下文环境
    virtual void on_work_started() ASIO_NOEXCEPT = 0; //通知executor有一些未完成的work去做
    virtual void on_work_finished() ASIO_NOEXCEPT = 0; //通知executor一些work已经完成

    //请求executor 调用给定的函数对象
    virtual void dispatch(ASIO_MOVE_ARG(function)) = 0;

    //请求executor 调用给定的函数对象
    virtual void post(ASIO_MOVE_ARG(function)) = 0;
    //请求executor 调用给定的函数对象
    virtual void defer(ASIO_MOVE_ARG(function)) = 0;
    
    virtual type_id_result_type target_type() const ASIO_NOEXCEPT = 0;

    virtual void* target() ASIO_NOEXCEPT = 0;
    virtual const void* target() const ASIO_NOEXCEPT = 0;

    //判断给定的2个 executor是否相等
    virtual bool equals(const impl_base* e) const ASIO_NOEXCEPT = 0;

  protected:
    impl_base(bool fast_dispatch) : fast_dispatch_(fast_dispatch) {}
    virtual ~impl_base() {}

  private:
    friend class executor;
    const bool fast_dispatch_;
  };
```

### executor
- 不是模板类，是实际类
- 拥有 impl_base* impl_ 指针，可以实现多态
- 构造函数
  > executor():impl_(0)  
  > executor(nullptr_t):impl_(0)  
  > executor(const executor& other):impl_(other.clone())  
  > executor(executor&& other)  
  > 偏特化 template <typename Executor> executor(Executor e)


#### executor::function
- 轻量级，只能move copy的函数对象包装器
- 拥有一个 detail::executor_function_base* func_ 指针
- 构造函数
  ```c++
  template <typename F, typename Alloc>
  explicit function(F f, const Alloc& a)
  {
    // 类名
    typedef detail::executor_function<F, Alloc> func_type;
    typename func_type::ptr p = {
      detail::addressof(a), func_type::ptr::allocate(a), 0 };
    /*
        p.a = &a ; 分配器
        由 a 分配F的内存空间，指针由p.v保存
        p.p = nullptr;
    */
     // void * operator new（std :: size_t size，void * ptr）
     // 这种用法不分配内存，但是会构造
    func_ = new (p.v) func_type(ASIO_MOVE_CAST(F)(f), a);
    //v所指向的内存已经到func_中了
    p.v = 0;
  }
  ```
- 函数调用
```c++
  void operator()()
  {
    if (func_)
    {
      detail::executor_function_base* func = func_;
      func_ = 0;
      func->complete();
    }
  }
```
#### executor::impl
- 默认的多态分配器实现类
```c++
template <typename Executor, typename Allocator>
class executor::impl
  : public executor::impl_base
{
public:
  typedef ASIO_REBIND_ALLOC(Allocator, impl) allocator_type;

  static impl_base* create(const Executor& e, Allocator a = Allocator())
  {
    raw_mem mem(a);
    impl* p = new (mem.ptr_) impl(e, a);
    mem.ptr_ = 0;
    return p;
  }

  impl(const Executor& e, const Allocator& a) ASIO_NOEXCEPT
    : impl_base(false),
      ref_count_(1),
      executor_(e),
      allocator_(a)
  {
  }

  impl_base* clone() const ASIO_NOEXCEPT
  {
    ++ref_count_;
    return const_cast<impl_base*>(static_cast<const impl_base*>(this));
  }

  void destroy() ASIO_NOEXCEPT
  {
    if (--ref_count_ == 0)
    {
      allocator_type alloc(allocator_);
      impl* p = this;
      p->~impl();
      alloc.deallocate(p, 1);
    }
  }

  void on_work_started() ASIO_NOEXCEPT
  {
    executor_.on_work_started();
  }

  void on_work_finished() ASIO_NOEXCEPT
  {
    executor_.on_work_finished();
  }

  execution_context& context() ASIO_NOEXCEPT
  {
    return executor_.context();
  }

  void dispatch(ASIO_MOVE_ARG(function) f)
  {
    executor_.dispatch(ASIO_MOVE_CAST(function)(f), allocator_);
  }

  void post(ASIO_MOVE_ARG(function) f)
  {
    executor_.post(ASIO_MOVE_CAST(function)(f), allocator_);
  }

  void defer(ASIO_MOVE_ARG(function) f)
  {
    executor_.defer(ASIO_MOVE_CAST(function)(f), allocator_);
  }

  type_id_result_type target_type() const ASIO_NOEXCEPT
  {
    return type_id<Executor>();
  }

  void* target() ASIO_NOEXCEPT
  {
    return &executor_;
  }

  const void* target() const ASIO_NOEXCEPT
  {
    return &executor_;
  }

  bool equals(const impl_base* e) const ASIO_NOEXCEPT
  {
    if (this == e)
      return true;
    if (target_type() != e->target_type())
      return false;
    return executor_ == *static_cast<const Executor*>(e->target());
  }

private:
  mutable detail::atomic_count ref_count_;
  Executor executor_;
  Allocator allocator_;

  struct raw_mem
  {
    allocator_type allocator_;
    impl* ptr_;

    explicit raw_mem(const Allocator& a)
      : allocator_(a),
        ptr_(allocator_.allocate(1))
    {
    }

    ~raw_mem()
    {
      if (ptr_)
        allocator_.deallocate(ptr_, 1);
    }

  private:
    // Disallow copying and assignment.
    raw_mem(const raw_mem&);
    raw_mem operator=(const raw_mem&);
  };
};
```




```c++
template <typename T>
class tss_ptr
  : public posix_tss_ptr<T>
#endif
{
public:
  void operator=(T* value)
  {
    posix_tss_ptr<T>::operator=(value);

  }
};

```

```c++
class thread_context
{
public:
  // Per-thread call stack to track the state of each thread in the context.
  typedef call_stack<thread_context, thread_info_base> thread_call_stack;
};
```
- 在context中，每个线程都调用 call stack 去跟踪每个线程的状态

- key 是一个对象指针，表示一个对象特有的value
- top_ 只分配一次,因此pthread_key_t 只会创建一个,采用一键多值的技术(thread_local)
```c++
thread_context,thread_info_base
class call_stack
{
public:
  class context
    : private noncopyable
  {

    // Push the key/value pair on to the stack.
    context(Key* k, Value& v)
      : key_(k),
        value_(&v),
        next_(call_stack<Key, Value>::top_)
    {
      call_stack<Key, Value>::top_ = this;
    }

    // Pop the key/value pair from the stack.
    ~context()
    {
      call_stack<Key, Value>::top_ = next_;
    }

    // Find the next context with the same key.
    Value* next_by_key() const
    {
      context* elem = next_;
      while (elem)
      {
        if (elem->key_ == key_)
          return elem->value_;
        elem = elem->next_;
      }
      return 0;
    }

  private:
    friend class call_stack<Key, Value>;

    // The key associated with the context.
    Key* key_;

    // The value associated with the context.
    Value* value_;

    // The next element in the stack.
    context* next_;
  };

private:
  // The top of the stack of calls for the current thread.
  static tss_ptr<context> top_;
};

template <typename Key, typename Value>
tss_ptr<typename call_stack<Key, Value>::context>
call_stack<Key, Value>::top_;
```

