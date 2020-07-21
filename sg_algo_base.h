//
// Created by dell on 2019/12/24.
//

#ifndef SG_STL_SG_ALGO_BASE_H
#define SG_STL_SG_ALGO_BASE_H

#include <cstring>
#include <type_traits>
#include <memory>
#include "sg_basic.h"

namespace sg {

	inline char* sg_copy(const char* begin, const char* end, char* result) {
		memmove(result, begin, end - begin);
		return result + (end - begin);
	}

	template <typename InputIter, typename OutputIter>
	OutputIter sg_copy(InputIter begin, InputIter end, OutputIter result) {
		while (begin != end) {
			*result++ = *begin++;
		}
		return begin;
	}

	template <typename InputIter, typename OutputIter>
	OutputIter sg_move(InputIter begin, InputIter end, OutputIter result) {
		while (begin != end) {
			*result++ = std::move(*begin++);
		}
		return result;
	}


	template <typename BidirectionalIter, typename BidirectionalIter1>
	BidirectionalIter1 sg_copy_backward(BidirectionalIter begin, BidirectionalIter end, BidirectionalIter1 result) {
		while (end != begin) {
			*--result = *--end;
		}
		return result;
	}

	template <typename BidirectionalIter, typename BidirectionalIter1>
	BidirectionalIter1 sg_move_backward(BidirectionalIter begin, BidirectionalIter end, BidirectionalIter1 result) {
		while (end != begin) {
			*--result = std::move(*--end);
		}
		return result;
	}


	template<typename Iter, typename T>
	Iter sg_find(Iter begin, Iter end, const T& value) {
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


	template<class ForwardIterator, class T>
	constexpr void sg_iota(ForwardIterator begin, ForwardIterator end, T value)
	{
		while (begin != end) {
			*begin++ = value;
			++value;
		}
	}

	template<class ForwardIterator, class T>
	constexpr void sg_fill(ForwardIterator begin, ForwardIterator end, const T& value)
	{
		while (begin != end) {
			*begin++ = value;
		}
	}




}

#endif //SG_STL_SG_ALGO_BASE_H
