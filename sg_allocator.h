//
// Created by dell on 2019/9/18.
//

#ifndef SG_STL_SG_ALLOCATOR_H
#define SG_STL_SG_ALLOCATOR_H

#include <new>
#include "sg_basic.h"
namespace sg {
	
	template< class T >
	static T* sg_addressof(T& arg)
	{
		return reinterpret_cast<T*>(
				&const_cast<char&>(
						reinterpret_cast<const volatile char&>(arg)));
	}
	template <typename T>
	static T* sg_allocate(const size_t& n = 1) {
		return static_cast<T*>(::operator new((sizeof(T) * n)));
	}

	template <typename T>
	static void sg_deallocate(T* p, size_type n = 1) {
		if (n != 0) {
			::operator delete(p);
		}
	}

	template <typename T, typename ...Args>
	static void sg_construct(T* p, Args&& ...args) {
		::new (static_cast<void*>(p)) T(std::forward<Args>(args)...);
	}

	template <typename T>
	static void sg_destroy (T* p) {
		p->~T();
	}
	
	template <typename T>
	inline void sg_destroy_at(T *p) {
		p->~T();
	}
	
	
	template <typename InputIter, typename ForwardIter>
	ForwardIter sg_uninitialized_copy(InputIter begin, InputIter end, ForwardIter result) {
		for (; begin != end; ++begin, ++result) {
			sg_construct(sg_addressof(*result), *begin);
		}
		return result;
	}
	template <typename InputIter, typename ForwardIter>
	ForwardIter sg_uninitialized_copy_n(InputIter begin, size_t n, ForwardIter result) {
		for (; n > 0; --n, ++begin, ++result) {
			sg_construct(sg_addressof(*result), *begin);
		}
		return result;
	}
	
	template <typename InputIter, typename ForwardIter>
	ForwardIter sg_uninitialized_move(InputIter begin, InputIter end, ForwardIter result) {
		for (; begin != end; ++begin, ++result) {
			sg_construct(sg_addressof(*result), std::move(*begin));
		}
		return result;
	}
	
	
	template <typename InputIter, typename ForwardIter>
	ForwardIter sg_uninitialized_move_n(InputIter begin, size_t n, ForwardIter result) {
		for (; n > 0; --n, ++begin) {
			sg_construct(sg_addressof(*result), std::move(*begin));
		}
		return result;
	}
	
	template <typename ForwardIter, typename T>
	ForwardIter sg_uninitialized_fill(ForwardIter begin, ForwardIter end, const T& value) {
		for (; begin != end; ++begin) {
			sg_construct(sg_addressof(*begin), value);
		}
		return begin;
	}
	
	template <typename ForwardIter, typename T>
	ForwardIter sg_uninitialized_fill_n(ForwardIter begin, size_t n, const T& value) {
		for (; n > 0; --n, ++begin) {
			sg_construct(sg_addressof(*begin), value);
		}
		return begin;
	}
	
	
	
	template <typename ForwardIter>
	ForwardIter sg_destroy(ForwardIter begin, ForwardIter end) {
		for (; begin != end; ++begin) {
			sg_destroy(sg_addressof(*begin));
		}
		return begin;
	}
	
	
}

#endif //SG_STL_SG_ALLOCATOR_H
