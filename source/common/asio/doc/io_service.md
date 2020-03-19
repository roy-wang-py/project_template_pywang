- io_service ---> io_context (io_context.hpp)
- io_context 继承自 execution_context

## service_registry
- 用于服务注册管理，只是提供一个框架
- 使用 execution_context::service* 形成单链表
-  service_registry 类型的对象有一个还是有多个? 
    > 一个
- 

```c++
void service_registry::do_add_service(
    const execution_context::service::key& key,
    execution_context::service* new_service)
{
    //如果该服务的owner 和 新服务的owner不一样，抛出异常
    //这说明所有的owner都一样
  if (&owner_ != &new_service->context())
    asio::detail::throw_exception(invalid_service_owner());

  asio::detail::mutex::scoped_lock lock(mutex_);

  // Check if there is an existing service object with the given key.
  execution_context::service* service = first_service_;
  while (service)
  {
    if (keys_match(service->key_, key))
      asio::detail::throw_exception(service_already_exists());
    service = service->next_;
  }

  // Take ownership of the service object.
  new_service->key_ = key;
  new_service->next_ = first_service_;
  first_service_ = new_service;
}

```


### execution_context::service
- 内部有execution_context& owner ，表示 该服务的拥有者
- 内部key 结构体 ，type_info 唯一的表示一个类型的特征
  > ```c++
  >struct key
  >{
  >key() : type_info_(0), id_(0) {}
  >const std::type_info* type_info_;
   > const execution_context::id* id_;
  >} key_
  >```



## execution_context
- 一个可执行上下文(`execution context`)表示函数对象可以在此处被执行。`io_context` 是一个可执行上下文 (`execution context`) 的例子。
- 类 execution context 实现了一个可扩展的、类型安全的、多态化的服务集合，此集合可根据服务类型索引。
- 服务存在的原因是去管理在一个可执行上下文中被共享的资源。
- id 结构体是一个空的结构体,可以唯一的表示一个executor_context的





## io_context
- io_context 类提供给用户异步操作 I/O 对象的核心能力，包括:  
    - asio::ip::tcp::socket
    - asio::ip::tcp::acceptor
    - asio::ip::udp::socket
    - asio::deadline_timer
- 线程安全


