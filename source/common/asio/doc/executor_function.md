# executor_function
## executor_function_base
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

- 成员变量是一个函数指针，该函数原型
  >typedef void (*func_type)(executor_function_base*, bool)
  - trul: complete操作
  - false:destory操作


## executor_function

- 批量定义指针 ASIO_DEFINE_TAGGED_HANDLER_ALLOCATOR_PTR



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
  Function function_;
  Alloc allocator_;
}
```