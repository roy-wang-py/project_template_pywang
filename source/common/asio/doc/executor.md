# executor

## class impl_base,抽象类，作为具体实现类的抽象接口

```c++
  class impl_base
  {
  public:
    virtual impl_base* clone() const ASIO_NOEXCEPT = 0;
    virtual void destroy() ASIO_NOEXCEPT = 0;
    virtual execution_context& context() ASIO_NOEXCEPT = 0;
    virtual void on_work_started() ASIO_NOEXCEPT = 0;
    virtual void on_work_finished() ASIO_NOEXCEPT = 0;
    virtual void dispatch(ASIO_MOVE_ARG(function)) = 0;
    virtual void post(ASIO_MOVE_ARG(function)) = 0;
    virtual void defer(ASIO_MOVE_ARG(function)) = 0;
    virtual type_id_result_type target_type() const ASIO_NOEXCEPT = 0;
    virtual void* target() ASIO_NOEXCEPT = 0;
    virtual const void* target() const ASIO_NOEXCEPT = 0;
    virtual bool equals(const impl_base* e) const ASIO_NOEXCEPT = 0;

  protected:
    impl_base(bool fast_dispatch) : fast_dispatch_(fast_dispatch) {}
    virtual ~impl_base() {}

  private:
    friend class executor;
    const bool fast_dispatch_;
  };
```
## impl
- 继承自 impl_base，含有 executor对象，实现来具体的函数调用

## class executor::function
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
    func_ = new (p.v) func_type(ASIO_MOVE_CAST(F)(f), a);
    p.v = 0;
  }

  function(function&& other) ASIO_NOEXCEPT
    : func_(other.func_)
  {
    other.func_ = 0;
  }

  ~function()
  {
    if (func_)
      func_->destroy();
  }

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
  detail::executor_function_base* func_;
};
```

## class executor

```c++
class executor{
    private: 
        impl_base* impl_;
}
```
- impl_ 作为接口对象完成具体的操作