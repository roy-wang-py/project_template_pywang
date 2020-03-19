

# io_object_impl

```c++
template <typename IoObjectService,
    typename Executor = io_context::executor_type>
class io_object_impl
{

  public:
  // The type of the service that will be used to provide I/O operations.
  typedef IoObjectService service_type;

  // The underlying implementation type of I/O object.
  typedef typename service_type::implementation_type implementation_type;

  // The type of the executor associated with the object.
  typedef Executor executor_type;

  // The type of executor to be used when implementing asynchronous operations.
  typedef io_object_executor<Executor> implementation_executor_type;
    private:

  // The service associated with the I/O object.
  service_type* service_;

  // The underlying implementation of the I/O object.
  implementation_type implementation_;

  // The associated executor.
  implementation_executor_type implementation_executor_;


  public:

  // Construct an I/O object using an executor.
  explicit io_object_impl(const executor_type& ex)
    : service_(&asio::use_service<IoObjectService>(ex.context())),
      implementation_executor_(ex, (is_native_io_executor)(ex))
  {
    service_->construct(implementation_);
  }

  // Construct an I/O object using an execution context.
  template <typename ExecutionContext>
  explicit io_object_impl(ExecutionContext& context,
      typename enable_if<is_convertible<
        ExecutionContext&, execution_context&>::value>::type* = 0)
    : service_(&asio::use_service<IoObjectService>(context)),
      implementation_executor_(context.get_executor(),
        is_same<ExecutionContext, io_context>::value)
  {
    service_->construct(implementation_);
  }

#if defined(ASIO_HAS_MOVE)
  // Move-construct an I/O object.
  io_object_impl(io_object_impl&& other)
    : service_(&other.get_service()),
      implementation_executor_(other.get_implementation_executor())
  {
    service_->move_construct(implementation_, other.implementation_);
  }

  // Perform a converting move-construction of an I/O object.
  template <typename IoObjectService1, typename Executor1>
  io_object_impl(io_object_impl<IoObjectService1, Executor1>&& other)
    : service_(&asio::use_service<IoObjectService>(
            other.get_implementation_executor().context())),
      implementation_executor_(other.get_implementation_executor())
  {
    service_->converting_move_construct(implementation_,
        other.get_service(), other.get_implementation());
  }
#endif // defined(ASIO_HAS_MOVE)

  // Destructor.
  ~io_object_impl()
  {
    service_->destroy(implementation_);
  }

#if defined(ASIO_HAS_MOVE)
  // Move-assign an I/O object.
  io_object_impl& operator=(io_object_impl&& other)
  {
    if (this != &other)
    {
      service_->move_assign(implementation_,
          *other.service_, other.implementation_);
      implementation_executor_.~implementation_executor_type();
      new (&implementation_executor_) implementation_executor_type(
          std::move(other.implementation_executor_));
      service_ = other.service_;
    }
    return *this;
  }
#endif // defined(ASIO_HAS_MOVE)

  // Get the executor associated with the object.
  executor_type get_executor() ASIO_NOEXCEPT
  {
    return implementation_executor_.inner_executor();
  }

  // Get the executor to be used when implementing asynchronous operations.
  const implementation_executor_type& get_implementation_executor()
    ASIO_NOEXCEPT
  {
    return implementation_executor_;
  }

  // Get the service associated with the I/O object.
  service_type& get_service()
  {
    return *service_;
  }

  // Get the service associated with the I/O object.
  const service_type& get_service() const
  {
    return *service_;
  }

  // Get the underlying implementation of the I/O object.
  implementation_type& get_implementation()
  {
    return implementation_;
  }

  // Get the underlying implementation of the I/O object.
  const implementation_type& get_implementation() const
  {
    return implementation_;
  }


};

```