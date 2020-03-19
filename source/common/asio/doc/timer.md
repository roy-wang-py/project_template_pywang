# timer

## basic_waitable_timer

- basic_waitable_timer 类模板 提供来执行阻塞或者异步等待一个定时器过期的能力
- 一个可等待定时器总是两种状态之一：失效或者未失效， 如果在一个定失效的定时器上调用 wait() or asyn_wait()，该操作将立马完成。
- 多数应用将使用 asio::steady_timer,asio::system_timer 或者 asio::high_resolution_timer 中的一个。
- 线程安全
- 不能拷贝构造和赋值构造
- using high_resolution_clock = system_clock