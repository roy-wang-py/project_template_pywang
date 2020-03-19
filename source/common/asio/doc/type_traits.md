# type_traits 类型萃取
## allocator 内存分配器

### 普通类型的分配器
```c++
  template<typename _Tp>
    class allocator: public __allocator_base<_Tp>
    {
   public:
      typedef size_t     size_type;
      typedef ptrdiff_t  difference_type;
      typedef _Tp*       pointer;
      typedef const _Tp* const_pointer;
      typedef _Tp&       reference;
      typedef const _Tp& const_reference;
      typedef _Tp        value_type;

    // 重新绑定
      template<typename _Tp1>
	struct rebind
	{ typedef allocator<_Tp1> other; };

#if __cplusplus >= 201103L
      // _GLIBCXX_RESOLVE_LIB_DEFECTS
      // 2103. std::allocator propagate_on_container_move_assignment
      typedef true_type propagate_on_container_move_assignment;

      typedef true_type is_always_equal;
#endif

      allocator() throw() { }

      allocator(const allocator& __a) throw()
      : __allocator_base<_Tp>(__a) { }

      template<typename _Tp1>
	allocator(const allocator<_Tp1>&) throw() { }

      ~allocator() throw() { }

      // Inherit everything else.
    };

```

## allocator_traits 
- allocator_traits 类模板提供访问分配器 (Allocator) 各种属性的标准化方式。标准容器和其他标准库组件通过此模板访问分配器，这使得能以任何类类型为分配器，只要用户提供的 allocator_traits 特化实现所有要求的功能。

### __allocator_traits_base

- _TP 是指 allocator，__allocator_traits_base 是统一各个分配器内部类型的名称
```c++
struct __allocator_traits_base
  {
    template<typename _Tp, typename _Up, typename = void>
      struct __rebind : __replace_first_arg<_Tp, _Up> { };

    template<typename _Tp, typename _Up>
      struct __rebind<_Tp, _Up,
		      __void_t<typename _Tp::template rebind<_Up>::other>>
      { using type = typename _Tp::template rebind<_Up>::other; };

  protected:
    template<typename _Tp>
      using __pointer = typename _Tp::pointer;
    template<typename _Tp>
      using __c_pointer = typename _Tp::const_pointer;
    template<typename _Tp>
      using __v_pointer = typename _Tp::void_pointer;
    template<typename _Tp>
      using __cv_pointer = typename _Tp::const_void_pointer;
    template<typename _Tp>
      using __pocca = typename _Tp::propagate_on_container_copy_assignment;
    template<typename _Tp>
      using __pocma = typename _Tp::propagate_on_container_move_assignment;
    template<typename _Tp>
      using __pocs = typename _Tp::propagate_on_container_swap;
    template<typename _Tp>
      using __equal = typename _Tp::is_always_equal;
  };

  template<typename _Alloc, typename _Up>
    using __alloc_rebind
      = typename __allocator_traits_base::template __rebind<_Alloc, _Up>::type;

```

### allocator_traits
- 对所有的allocator类型归一化接口

```c++
 template<typename _Alloc>
    struct allocator_traits : __allocator_traits_base
    {
      typedef _Alloc allocator_type; //分配器类型
      typedef typename _Alloc::value_type value_type; // 分配器所分配的类型

      /**
       * @brief   The allocator's pointer type.
       *
       * @c Alloc::pointer if that type exists, otherwise @c value_type*
      */
      using pointer = __detected_or_t<value_type*, __pointer, _Alloc>;

    private:
      // Select _Func<_Alloc> or pointer_traits<pointer>::rebind<_Tp>
      template<template<typename> class _Func, typename _Tp, typename = void>
	struct _Ptr
	{
	  using type = typename pointer_traits<pointer>::template rebind<_Tp>;
	};

      template<template<typename> class _Func, typename _Tp>
	struct _Ptr<_Func, _Tp, __void_t<_Func<_Alloc>>>
	{
	  using type = _Func<_Alloc>;
	};

      // Select _A2::difference_type or pointer_traits<_Ptr>::difference_type
      template<typename _A2, typename _PtrT, typename = void>
	struct _Diff
	{ using type = typename pointer_traits<_PtrT>::difference_type; };

      template<typename _A2, typename _PtrT>
	struct _Diff<_A2, _PtrT, __void_t<typename _A2::difference_type>>
	{ using type = typename _A2::difference_type; };

      // Select _A2::size_type or make_unsigned<_DiffT>::type
      template<typename _A2, typename _DiffT, typename = void>
	struct _Size : make_unsigned<_DiffT> { };

      template<typename _A2, typename _DiffT>
	struct _Size<_A2, _DiffT, __void_t<typename _A2::size_type>>
	{ using type = typename _A2::size_type; };

    public:
      /**
       * @brief   The allocator's const pointer type.
       *
       * @c Alloc::const_pointer if that type exists, otherwise
       * <tt> pointer_traits<pointer>::rebind<const value_type> </tt>
      */
      using const_pointer = typename _Ptr<__c_pointer, const value_type>::type;

      /**
       * @brief   The allocator's void pointer type.
       *
       * @c Alloc::void_pointer if that type exists, otherwise
       * <tt> pointer_traits<pointer>::rebind<void> </tt>
      */
      using void_pointer = typename _Ptr<__v_pointer, void>::type;

      /**
       * @brief   The allocator's const void pointer type.
       *
       * @c Alloc::const_void_pointer if that type exists, otherwise
       * <tt> pointer_traits<pointer>::rebind<const void> </tt>
      */
      using const_void_pointer = typename _Ptr<__cv_pointer, const void>::type;

      /**
       * @brief   The allocator's difference type
       *
       * @c Alloc::difference_type if that type exists, otherwise
       * <tt> pointer_traits<pointer>::difference_type </tt>
      */
      using difference_type = typename _Diff<_Alloc, pointer>::type;

      /**
       * @brief   The allocator's size type
       *
       * @c Alloc::size_type if that type exists, otherwise
       * <tt> make_unsigned<difference_type>::type </tt>
      */
      using size_type = typename _Size<_Alloc, difference_type>::type;

      /**
       * @brief   How the allocator is propagated on copy assignment
       *
       * @c Alloc::propagate_on_container_copy_assignment if that type exists,
       * otherwise @c false_type
      */
      using propagate_on_container_copy_assignment
	= __detected_or_t<false_type, __pocca, _Alloc>;

      /**
       * @brief   How the allocator is propagated on move assignment
       *
       * @c Alloc::propagate_on_container_move_assignment if that type exists,
       * otherwise @c false_type
      */
      using propagate_on_container_move_assignment
	= __detected_or_t<false_type, __pocma, _Alloc>;

      /**
       * @brief   How the allocator is propagated on swap
       *
       * @c Alloc::propagate_on_container_swap if that type exists,
       * otherwise @c false_type
      */
      using propagate_on_container_swap
	= __detected_or_t<false_type, __pocs, _Alloc>;

      /**
       * @brief   Whether all instances of the allocator type compare equal.
       *
       * @c Alloc::is_always_equal if that type exists,
       * otherwise @c is_empty<Alloc>::type
      */
      using is_always_equal
	= __detected_or_t<typename is_empty<_Alloc>::type, __equal, _Alloc>;

      template<typename _Tp>
	using rebind_alloc = __alloc_rebind<_Alloc, _Tp>;
      template<typename _Tp>
	using rebind_traits = allocator_traits<rebind_alloc<_Tp>>;

    private:
      template<typename _Alloc2>
	static auto
	_S_allocate(_Alloc2& __a, size_type __n, const_void_pointer __hint, int)
	-> decltype(__a.allocate(__n, __hint))
	{ return __a.allocate(__n, __hint); }

      template<typename _Alloc2>
	static pointer
	_S_allocate(_Alloc2& __a, size_type __n, const_void_pointer, ...)
	{ return __a.allocate(__n); }

      template<typename _Tp, typename... _Args>
	struct __construct_helper
	{
	  template<typename _Alloc2,
	    typename = decltype(std::declval<_Alloc2*>()->construct(
		  std::declval<_Tp*>(), std::declval<_Args>()...))>
	    static true_type __test(int);

	  template<typename>
	    static false_type __test(...);

	  using type = decltype(__test<_Alloc>(0));
	};

      template<typename _Tp, typename... _Args>
	using __has_construct
	  = typename __construct_helper<_Tp, _Args...>::type;

      template<typename _Tp, typename... _Args>
	static _Require<__has_construct<_Tp, _Args...>>
	_S_construct(_Alloc& __a, _Tp* __p, _Args&&... __args)
	{ __a.construct(__p, std::forward<_Args>(__args)...); }

      template<typename _Tp, typename... _Args>
	static
	_Require<__and_<__not_<__has_construct<_Tp, _Args...>>,
			       is_constructible<_Tp, _Args...>>>
	_S_construct(_Alloc&, _Tp* __p, _Args&&... __args)
	{ ::new((void*)__p) _Tp(std::forward<_Args>(__args)...); }

      template<typename _Alloc2, typename _Tp>
	static auto
	_S_destroy(_Alloc2& __a, _Tp* __p, int)
	-> decltype(__a.destroy(__p))
	{ __a.destroy(__p); }

      template<typename _Alloc2, typename _Tp>
	static void
	_S_destroy(_Alloc2&, _Tp* __p, ...)
	{ __p->~_Tp(); }

      template<typename _Alloc2>
	static auto
	_S_max_size(_Alloc2& __a, int)
	-> decltype(__a.max_size())
	{ return __a.max_size(); }

      template<typename _Alloc2>
	static size_type
	_S_max_size(_Alloc2&, ...)
	{
	  // _GLIBCXX_RESOLVE_LIB_DEFECTS
	  // 2466. allocator_traits::max_size() default behavior is incorrect
	  return __gnu_cxx::__numeric_traits<size_type>::__max
	    / sizeof(value_type);
	}

      template<typename _Alloc2>
	static auto
	_S_select(_Alloc2& __a, int)
	-> decltype(__a.select_on_container_copy_construction())
	{ return __a.select_on_container_copy_construction(); }

      template<typename _Alloc2>
	static _Alloc2
	_S_select(_Alloc2& __a, ...)
	{ return __a; }

    public:

      /**
       *  @brief  Allocate memory.
       *  @param  __a  An allocator.
       *  @param  __n  The number of objects to allocate space for.
       *
       *  Calls @c a.allocate(n)
      */
      static pointer
      allocate(_Alloc& __a, size_type __n)
      { return __a.allocate(__n); }

      /**
       *  @brief  Allocate memory.
       *  @param  __a  An allocator.
       *  @param  __n  The number of objects to allocate space for.
       *  @param  __hint Aid to locality.
       *  @return Memory of suitable size and alignment for @a n objects
       *          of type @c value_type
       *
       *  Returns <tt> a.allocate(n, hint) </tt> if that expression is
       *  well-formed, otherwise returns @c a.allocate(n)
      */
      static pointer
      allocate(_Alloc& __a, size_type __n, const_void_pointer __hint)
      { return _S_allocate(__a, __n, __hint, 0); }

      /**
       *  @brief  Deallocate memory.
       *  @param  __a  An allocator.
       *  @param  __p  Pointer to the memory to deallocate.
       *  @param  __n  The number of objects space was allocated for.
       *
       *  Calls <tt> a.deallocate(p, n) </tt>
      */
      static void
      deallocate(_Alloc& __a, pointer __p, size_type __n)
      { __a.deallocate(__p, __n); }

      /**
       *  @brief  Construct an object of type @a _Tp
       *  @param  __a  An allocator.
       *  @param  __p  Pointer to memory of suitable size and alignment for Tp
       *  @param  __args Constructor arguments.
       *
       *  Calls <tt> __a.construct(__p, std::forward<Args>(__args)...) </tt>
       *  if that expression is well-formed, otherwise uses placement-new
       *  to construct an object of type @a _Tp at location @a __p from the
       *  arguments @a __args...
      */
      template<typename _Tp, typename... _Args>
	static auto construct(_Alloc& __a, _Tp* __p, _Args&&... __args)
	-> decltype(_S_construct(__a, __p, std::forward<_Args>(__args)...))
	{ _S_construct(__a, __p, std::forward<_Args>(__args)...); }

      /**
       *  @brief  Destroy an object of type @a _Tp
       *  @param  __a  An allocator.
       *  @param  __p  Pointer to the object to destroy
       *
       *  Calls @c __a.destroy(__p) if that expression is well-formed,
       *  otherwise calls @c __p->~_Tp()
      */
      template<typename _Tp>
	static void destroy(_Alloc& __a, _Tp* __p)
	{ _S_destroy(__a, __p, 0); }

      /**
       *  @brief  The maximum supported allocation size
       *  @param  __a  An allocator.
       *  @return @c __a.max_size() or @c numeric_limits<size_type>::max()
       *
       *  Returns @c __a.max_size() if that expression is well-formed,
       *  otherwise returns @c numeric_limits<size_type>::max()
      */
      static size_type max_size(const _Alloc& __a) noexcept
      { return _S_max_size(__a, 0); }

      /**
       *  @brief  Obtain an allocator to use when copying a container.
       *  @param  __rhs  An allocator.
       *  @return @c __rhs.select_on_container_copy_construction() or @a __rhs
       *
       *  Returns @c __rhs.select_on_container_copy_construction() if that
       *  expression is well-formed, otherwise returns @a __rhs
      */
      static _Alloc
      select_on_container_copy_construction(const _Alloc& __rhs)
      { return _S_select(__rhs, 0); }
    };
```