//
// Created by dell on 2019/9/29.
//

#ifndef SG_STL_SG_ALGORITHM_H
#define SG_STL_SG_ALGORITHM_H



#include <cstring>
#include <type_traits>
#include <memory>
#include "sg_basic.h"

namespace sg{
	
	
	
	
	

	inline char * sg_copy(const char *begin, const char *end, char *result) {
		memmove(result, begin, end - begin);
		return result + (end - begin);
	}
	template <typename InputIter, typename OutputIter>
	OutputIter sg___copy_d(InputIter begin, InputIter end, OutputIter result ) {
		for (; begin != end; ++begin, ++result) {
			*result = *begin;
		}
		return begin;
	}
	
	template <typename InputIter, typename OutputIter>
	OutputIter sg_copy(InputIter begin, InputIter end, OutputIter result) {
		return sg___copy_d(begin, end, result);
	}


	template <typename BidirectionalIter>
	BidirectionalIter sg_copy_backward(BidirectionalIter begin, BidirectionalIter end, BidirectionalIter result) {
		for (; end != begin; --end, --result) {
			*(result - 1) = *(end - 1);
		}
	}

	template<typename Iter, typename T>
	Iter sg_find(Iter begin, Iter end, const T &value) {
		while (begin != end && *begin != value) {
			++begin;
		}
		return begin;
	}

	template <typename Iter, typename Func>
	Iter sg_find_if(Iter begin, Iter end, Func func) {
		while (begin != end && !func(*begin)) {
			++begin;
		}
		return begin;
	}

	template<typename Iter, typename Func>
	Func sg_for_each(Iter begin, Iter end, Func func) {
		while (begin != end) {
			func(*begin);
			++begin;
		}
		return func;
	}


}
#endif //SG_STL_SG_ALGORITHM_H
