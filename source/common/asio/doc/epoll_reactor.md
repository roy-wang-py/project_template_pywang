# epoll_reactor

## eventfd_select_interrupter
- 构造的一个可以中断机制，即构建一个pipe，把read fd添加到epoll中，在write fd中写1,那么epoll会被激活。
- 作用：打破一个阻塞的 epoll_wait 调用
## epoll event
```c++
int epoll_create (int __size)
int epoll_create (int __flags)
int epoll_ctl (int __epfd, int __op, int __fd,struct epoll_event *__event)
int epoll_wait (int __epfd, struct epoll_event *__events,int __maxevents, int __timeout)
```
- timeout = -1 时，表示无限期等待
- timeout = 0 时，表示不阻塞
## descriptor_state
- descriptor_state::perform_io 取出对应的事件，由scheduler 投递到完成队列中
- do_complete调用perform_io，并且就地把first_op 调用 complete，完成操作
## epoll_reactor

- 每一个epoll_reactor 都会有独立的epollfd和timerfd
- 使用同一个scheduler
  >scheduler_(use_service<scheduler>(ctx))

- 只有运行run() 接口后才能进行epoll_wait操作
- 构造函数中进行了一次interrupt
- run接口,
  >run(long usec, op_queue<operation>& ops)
  >如果传入的usec 为0,表示不阻塞，则epoll_wait立即返回
  >如果传入的usec 为-1,表示不超时，则等待到有活动
  >其他时间(us)，如果没有定时器fd，则获取到定时器最近的超时时间
  >获取epoll_wait返回的事件后，进行dispatch
  >如果为中断事件,不需要reset，因为是edge trigger
  >timer 事件，把ready timer op push到ops中
  >其他的文件描述符事件 进入到 descriptor_state
 

