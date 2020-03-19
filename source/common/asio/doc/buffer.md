# BUFFER 
- 缓冲区的指针由外界控制，buffer 类不控制
## 1. mutable_buffer
```c++
class mutable_buffer{
    private:
        void * data_;
        std::size_t size_;
}
```
- 默认构造函数
  >```c++
  > mutable_buffer():data_(0),size_(0){}
- 构造函数
  >```c++
  >mutable_buffer(void* data, std::size_t size):data_(data),size_(size){}
- 获取缓冲区指针
  >```c++
  >void* data() const {return data_;}
- 获取缓冲区大小
  >```c++
  >std::size_t size(){return size_;}
- 获得缓冲区的子集
   >```c++
    >mutable_buffer& operator+=(std::size_t n) ASIO_NOEXCEPT
    >{
      >std::size_t offset = n < size_ ? n : size_;
      >data_ = static_cast<char*>(data_) + offset;
      >size_ -= offset;
      >return *this;
    >}
 

## 2. const_buffer
- 缓冲区内容不能修改(const)
```c++
class mutable_buffer{
    private:
        const void * data_;
        std::size_t size_;
}
```
- 默认构造函数
  >```c++
  > mutable_buffer():data_(0),size_(0){}
- 构造函数
  >```c++
  >mutable_buffer(const void* data, std::size_t size):data_(data),size_(size){}
- copy 构造函数,可变缓冲区构造常量缓冲区
  >```c++
  >const_buffer(const mutable_buffer& b):data_(b.data()),size_(b.size()){}
  >
- 获取缓冲区指针
  >```c++
  >const void* data() const {return data_;}
- 获取缓冲区大小
  >```c++
  >std::size_t size(){return size_;}
- 获得缓冲区的子集
   >```c++
    >mutable_buffer& operator+=(std::size_t n) ASIO_NOEXCEPT
    >{
      >std::size_t offset = n < size_ ? n : size_;
      >data_ = static_cast<const char*>(data_) + offset;
      >size_ -= offset;
      >return *this;
    >}

## 3. 迭代器
### begin
- 获取一个引用对象mutable_buffer or const_buffer 的地址(指针) 
 ```c++
template <typename MutableBuffer>
inline const mutable_buffer* buffer_sequence_begin(const MutableBuffer& b,
    typename enable_if<
      is_convertible<const MutableBuffer*, const mutable_buffer*>::value
    >::type* = 0) ASIO_NOEXCEPT
{
  return static_cast<const mutable_buffer*>(detail::addressof(b));
}
 ```
```c++
template <typename ConstBuffer>
inline const const_buffer* buffer_sequence_begin(const ConstBuffer& b,
    typename enable_if<
      is_convertible<const ConstBuffer*, const const_buffer*>::value
    >::type* = 0) ASIO_NOEXCEPT
{
  return static_cast<const const_buffer*>(detail::addressof(b));
}
```
- 正常的容器的迭代器，使用后置 return -> decltype自动推导
```c++
template <typename C>
inline auto buffer_sequence_begin(C& c,
    typename enable_if<
      !is_convertible<const C*, const mutable_buffer*>::value
        && !is_convertible<const C*, const const_buffer*>::value
    >::type* = 0) ASIO_NOEXCEPT -> decltype(c.begin())
{
  return c.begin();
}
```
- 常引用
```c++
template <typename C>
inline auto buffer_sequence_begin(const C& c,
    typename enable_if<
      !is_convertible<const C*, const mutable_buffer*>::value
        && !is_convertible<const C*, const const_buffer*>::value
    >::type* = 0) ASIO_NOEXCEPT -> decltype(c.begin())
{
  return c.begin();
}
```
### end
```c++
template <typename MutableBuffer>
inline const mutable_buffer* buffer_sequence_end(const MutableBuffer& b,
    typename enable_if<
      is_convertible<const MutableBuffer*, const mutable_buffer*>::value
    >::type* = 0) ASIO_NOEXCEPT
{
  return static_cast<const mutable_buffer*>(detail::addressof(b)) + 1;
}
```
```c++
template <typename ConstBuffer>
inline const const_buffer* buffer_sequence_end(const ConstBuffer& b,
    typename enable_if<
      is_convertible<const ConstBuffer*, const const_buffer*>::value
    >::type* = 0) ASIO_NOEXCEPT
{
  return static_cast<const const_buffer*>(detail::addressof(b)) + 1;
}
```
```c++

template <typename C>
inline auto buffer_sequence_end(const C& c,
    typename enable_if<
      !is_convertible<const C*, const mutable_buffer*>::value
        && !is_convertible<const C*, const const_buffer*>::value
    >::type* = 0) ASIO_NOEXCEPT -> decltype(c.end())
{
  return c.end();
}
```
```c++
template <typename C>
inline auto buffer_sequence_end(C& c,
    typename enable_if<
      !is_convertible<const C*, const mutable_buffer*>::value
        && !is_convertible<const C*, const const_buffer*>::value
    >::type* = 0) ASIO_NOEXCEPT -> decltype(c.end())
{
  return c.end();
}

```