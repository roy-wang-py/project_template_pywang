#ifndef __SQHAO_BUFFER_HXX__
#define __SQHAO_BUFFER_HXX__



#include "Exception.hxx"
#include "bugcheck.hxx"
#include "default_config.hxx"
#include <cstring>
#include <cstddef>


NAMESPACE_PREFIX

template <class T>
class Buffer
{
public:
	Buffer(std::size_t capacity):_capacity(capacity),_used(capacity),_ptr(0),_ownMem(true){
		if (capacity > 0)
		{
			_ptr = new T[capacity];
		}
	}

	Buffer(T* pMem, std::size_t length):_capacity(length),_used(length),_ptr(pMem),_ownMem(false){}

	Buffer(const T* pMem, std::size_t length):_capacity(length),_used(length),_ptr(0),_ownMem(true){
		if (_capacity > 0)
		{
			_ptr = new T[_capacity];
			std::memcpy(_ptr, pMem, _used * sizeof(T));
		}
	}

	Buffer(const Buffer& other):_capacity(other._used),_used(other._used),_ptr(0),_ownMem(true){
		if (_used)
		{
			_ptr = new T[_used];
			std::memcpy(_ptr, other._ptr, _used * sizeof(T));
		}
	}

	Buffer& operator = (const Buffer& other){
		if (this != &other)
		{
			Buffer tmp(other);
			swap(tmp);
		}

		return *this;
	}

	~Buffer(){
		if (_ownMem) delete [] _ptr;
	}
	
	void resize(std::size_t newCapacity, bool preserveContent = true){
		if (!_ownMem) throw InvalidAccessException("Cannot resize buffer which does not own its storage.");

		if (newCapacity > _capacity)
		{
			T* ptr = new T[newCapacity];
			if (preserveContent)
			{
				std::memcpy(ptr, _ptr, _used * sizeof(T));
			}
			delete [] _ptr;
			_ptr = ptr;
			_capacity = newCapacity;
		}
		
		_used = newCapacity;
	}
	
	void setCapacity(std::size_t newCapacity, bool preserveContent = true){
		if (!_ownMem) throw InvalidAccessException("Cannot resize buffer which does not own its storage.");

		if (newCapacity != _capacity)
		{
			T* ptr = 0;
			if (newCapacity > 0)
			{
				ptr = new T[newCapacity];
				if (preserveContent)
				{
					std::size_t newSz = _used < newCapacity ? _used : newCapacity;
					std::memcpy(ptr, _ptr, newSz * sizeof(T));
				}
			}
			delete [] _ptr;
			_ptr = ptr;
			_capacity = newCapacity;

			if (newCapacity < _used) _used = newCapacity;
		}
	}

	void assign(const T* buf, std::size_t sz){
		if (0 == sz) return;
		if (sz > _capacity) resize(sz, false);
		std::memcpy(_ptr, buf, sz * sizeof(T));
		_used = sz;
	}

	void append(const T* buf, std::size_t sz){
		if (0 == sz) return;
		resize(_used + sz, true);
		std::memcpy(_ptr + _used - sz, buf, sz * sizeof(T));
	}

	void append(T val){
		resize(_used + 1, true);
		_ptr[_used - 1] = val;
	}

	void append(const Buffer& buf){
		append(buf.begin(), buf.size());
	}

	std::size_t capacity() const{
		return _capacity;
	}

	std::size_t capacityBytes() const{
		return _capacity * sizeof(T);
	}

	void swap(Buffer& other){
		using std::swap;
		swap(_ptr, other._ptr);
		swap(_capacity, other._capacity);
		swap(_used, other._used);
	}

	bool operator == (const Buffer& other) const{
		if (this != &other)
		{
			if (_used == other._used)
			{
				if (std::memcmp(_ptr, other._ptr, _used * sizeof(T)) == 0)
				{
					return true;
				}
			}
			return false;
		}

		return true;
	}

	bool operator != (const Buffer& other) const{
		return !(*this == other);
	}

	void clear(){
		std::memset(_ptr, 0, _used * sizeof(T));
	}

	std::size_t size() const{
		return _used;
	}

	std::size_t sizeBytes() const{
		return _used * sizeof(T);
	}
	
	T* begin(){
		return _ptr;
	}
	
	const T* begin() const{
		return _ptr;
	}

	T* end(){
		return _ptr + _used;
	}
	
	const T* end() const{
		return _ptr + _used;
	}
	
	bool empty() const{
		return 0 == _used;
	}

	T& operator [] (std::size_t index){
		common_assert(index < _used);
		return _ptr[index];
	}

	const T& operator [] (std::size_t index) const
	{
		common_assert(index < _used);
		return _ptr[index];
	}

private:
	Buffer()=delete;
	std::size_t _capacity;
	std::size_t _used;
	T*          _ptr;
	bool        _ownMem;
};


NAMESPACE_SUBFIX


#endif // __SQHAO_BUFFER_HXX__
