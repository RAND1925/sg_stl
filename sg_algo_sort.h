#ifndef SG_STL_SORTER_H
#define SG_STL_SORTER_H

#include <cassert>
#include "sg_allocator.h"
#include "sg_algo_base.h"
#include "sg_algo_heap.h"

namespace sg {
	template <typename ForwardIter, typename Comp>
	constexpr ForwardIter sg_is_sorted_until(ForwardIter begin, ForwardIter end, Comp comp) {
		if (begin != end) {
			auto next = begin;
			while (++next != end) {
				if (comp(*next, *begin)) {
					return next;
				}
				begin = next;
			}
		}
		return end;
	}
	
	template <typename ForwardIter>
	constexpr ForwardIter sg_is_sorted_until(ForwardIter begin, ForwardIter end) {
		return sg_is_sorted_until(begin, end, std::less<>());
	}
	
	template <typename ForwardIter, typename Comp>
	constexpr bool sg_is_sorted(ForwardIter begin, ForwardIter end, Comp comp) {
		return sg_is_sorted_until(begin, end, comp) == end;
	}
	
	template <typename ForwardIter>
	constexpr bool sg_is_sorted(ForwardIter begin, ForwardIter end) {
		return sg_is_sorted(begin, end, std::less<>());
	}
	
	template <typename RandomIter, typename Comp>
	constexpr void sg_select_sort(RandomIter begin, RandomIter end, Comp comp) {
		auto cbegin = begin; 
		for (; std::next(begin) != end; ++begin) {
			auto min_iter = begin;
			for (auto inner = std::next(begin); inner != end; ++inner) {
				if (comp(*inner, *min_iter)) {
					min_iter = inner;
				}
			} // 此时min即为最小的那个
			if (min_iter != begin) {
				std::swap(*min_iter, *begin);
			}
			assert(sg_is_sorted(cbegin, begin, comp));
		}
	}
	
	
	template <typename RandomIter>
	constexpr void sg_select_sort(RandomIter begin, RandomIter end) {
		sg_select_sort(begin, end, std::less<>());
	}
	
	
	template <typename RandomIter, typename Comp>
	constexpr void sg_bubble_sort(RandomIter begin, RandomIter end, Comp comp) {
		const auto cbegin = begin;
		++begin;
		for (; begin != end; ++begin) {
			bool swapped = false;
			for (auto inner = end - 1; inner != begin - 1; --inner) {
				if (comp(*inner, *(inner - 1))) {
					std::swap(*inner, *(inner - 1));
					swapped = true;
				}
			}
			if (!swapped) {
				assert(sg_is_sorted(cbegin, end));
				return;
			}
		}
	}
	
	template <typename RandomIter>
	constexpr void sg_bubble_sort(RandomIter begin, RandomIter end) {
		sg_bubble_sort(begin, end, std::less<>());
	}
	
	template <typename RandomIter, typename Comp>
	constexpr void sg_insertion_sort(RandomIter begin, RandomIter end, Comp comp) {
		const auto cbegin = begin;
		++begin;
		for (; begin != end; ++begin) {
			if (comp(*begin, *(begin - 1))) {
				auto value = std::move(*begin);
				if (comp(value, *cbegin)) {
					sg_move_backward(cbegin, begin, begin + 1);
					*cbegin = std::move(value);
				} else {
					auto inner = begin;
					do {
						*inner = std::move(*(inner - 1));
						--inner;
					} while (comp(value, *(inner - 1)));
					*inner = std::move(value);
				}
			}
			assert(sg_is_sorted(cbegin, begin));
		}
	}
	
	template <typename RandomIter>
	constexpr void sg_insertion_sort(RandomIter begin, RandomIter end) {
		sg_insertion_sort(begin, end, std::less<>());
	}
	
	
	
	// gap : gap = gap/2.2
	template <typename RandomIter, typename Comp>
	constexpr void sg_shell_sort(RandomIter begin, RandomIter end, Comp comp) {
		auto init_gap = [begin, end]() { return (end - begin) / 2.2; };
		auto update_gap = [](ptrdiff_t& gap) { gap /= 2.2; if (gap == 2) gap = 1; };
		const auto cbegin = begin;
		ptrdiff_t gap = init_gap();
		while (gap) {
			begin = cbegin + gap; // 间隔为gap的插入排序
			for (; begin != end; ++begin) {
				if (comp(*begin, *(begin - gap))) {
					auto value = std::move(*begin);
					auto inner = begin;
					do {
						*inner = std::move(*(inner - gap));
						inner -= gap;
					} while (inner >= gap + cbegin && comp(value, *(inner - gap)));
					*inner = std::move(value);
				}
			}
			update_gap(gap);
		}
	};
	
	template <typename RandomIter>
	constexpr void sg_shell_sort(RandomIter begin, RandomIter end) {
		sg_shell_sort(begin, end, std::less<>());
	}
	
	
	template <typename RandomIter, typename Comp>
	constexpr void sg_heap_sort(RandomIter begin, RandomIter end, Comp comp) {
		sg_make_heap(begin, end, comp); // 构造为最大堆
		assert(sg_is_heap(begin, end, comp));
		sg_sort_heap(begin, end, comp); 
	}
	
	template <typename RandomIter>
	constexpr void sg_heap_sort(RandomIter begin, RandomIter end) {
		sg_heap_sort(begin, end, std::less<>());
	}
	
	
	template <typename RandomIter, typename Pred>
	constexpr RandomIter sg_partition(RandomIter begin, RandomIter end, Pred pred) {
		while (begin != end && pred(*begin)) {
			++begin;
		}
		if (begin == end) {
			return begin;
		}
		for (auto iter = std::next(begin); iter != end; ++iter) {
			if (pred(*iter)) {
				std::iter_swap(iter, begin);
				++begin;
			}
		}
		return begin;
	}
	
	
	template <typename RandomIter, typename Comp>
	constexpr void sg_quick_sort(RandomIter begin, RandomIter end, Comp comp) {
		if (end - begin <= 1) {
			return;
		}
		const auto cbegin = begin;
		const auto cend = end;
		auto value{std::move(*cbegin)};
		--end;
		while (begin < end) {
			while (begin < end && comp(value, *end)) { // 找到第一个可以往前移动的节点
				--end;
			} 
			if (begin < end) { // 移动
				*begin = std::move(*end);
				++begin;
			}
			while (begin < end && comp(*begin, value)) {// 找到第一个可以往后移动的节点
				++begin;
			}
			if (begin < end) {// 移动
				*end = std::move(*begin);
				--end;
			}
		}
		*begin = std::move(value); // 最后将临时变量移入最后一个洞
		sg_quick_sort(cbegin, begin, comp); 
		sg_quick_sort(begin + 1, cend, comp);
	}
	
	template <typename RandomIter>
	constexpr void sg_quick_sort(RandomIter begin, RandomIter end) {
		sg_quick_sort(begin, end, std::less<>());
	}
	
	template <typename InputIter, typename InputIter1, typename OutputIter, typename Comp>
	OutputIter sg_merge(InputIter begin, InputIter end, InputIter1 begin1, InputIter1 end1,
			OutputIter res, Comp comp) {// 无关位置的归并
		while (begin != end) { 
			if (begin1 == end1) { // 如果一个序列结束，直接把另一个序列的移入
				return sg_move(begin, end, res);
			}
			if (comp(*begin1, *begin)) { // 找到较小的那个序列，移入res
				*res = std::move(*begin1);
				++begin1;
			} else {
				*res = std::move(*begin); // res ++
				++begin;
			}
			++res;
		}
		return sg_move(begin1, end1, res); // 如果一个序列结束，直接把另一个序列的移入，返回结果
	}
	
	template <typename RandomIter, typename Comp>
	void sg_inplace_merge(RandomIter begin, RandomIter middle, RandomIter end, Comp comp) { // 本地归并
		if (middle == begin || end == middle) {
			return;
		}
		using T = typename std::remove_reference<decltype(*begin)>::type;
		// 申请一段和middle - begin 一样大的内存，把前一半数据移入
		auto buffer_begin = sg_allocate<T>(middle - begin);
		auto buffer_end = sg_uninitialized_move(begin, middle, buffer_begin);
		sg_merge(buffer_begin, buffer_end, middle, end, begin, comp); // 归并
		sg_destroy(buffer_begin, buffer_end);
		sg_deallocate(buffer_begin, buffer_end - buffer_begin);
	}
	
	
	template <typename RandomIter, typename Comp>
	constexpr void sg_merge_sort(RandomIter begin, RandomIter end, Comp comp) {
		auto len = end - begin;
		if (len <= 1){ // 若长度小于等于1, 结束
			return;
		}
		auto middle = begin + len / 2;
		sg_merge_sort(begin, middle, comp); // 递归
		sg_merge_sort(middle, end, comp);
		sg_inplace_merge(begin, middle, end, comp);
	}
	
	template <typename RandomIter>
	constexpr void sg_merge_sort(RandomIter begin, RandomIter end) {
		sg_merge_sort(begin, end, std::less<>());
	}
	
	template <typename IntegerIter, ptrdiff_t radix= 10>
	void sg_radix_sort(IntegerIter begin, IntegerIter end){
		using Integer = typename std::remove_reference<decltype(*begin)>::type;
		const auto len = end - begin; // 计算长度
		auto front = sg_allocate<ptrdiff_t>(radix); // 申请front数组，记录链表结束，大小为radix，初始化为len
		sg_uninitialized_fill_n(front, radix, len); 
		auto rear = sg_allocate<ptrdiff_t>(radix); //  申请rear数组，记录链表开始，大小为radix，初始化为len
		sg_uninitialized_fill_n(rear, radix, len);
		ptrdiff_t start = 0; // 记录整个链表的开始和结束
		ptrdiff_t finish = len;
		auto link = sg_allocate<ptrdiff_t>(len); // 申请link数组，记录下一个节点
		sg_iota(link, link + len, 1); // 初始化为下标 + 1
		auto raw_array = sg_allocate<Integer>(len); // 复制原来的数据
		sg_uninitialized_copy(begin, end, raw_array);
		bool none_zero = true; // 记录是否已经全部为0
		while (none_zero) {
			none_zero = false; 
			for (auto i = start, next = link[i]; i != len; i = next) {
				size_t& k{ static_cast<size_t&>(begin[i] % radix) }; // 获得余数
				if (front[k] == len) { // 压入对应的桶中
					front[k] = i;
				} else {
					link[rear[k]] = i;
				}
				rear[k] = i;
				next = link[i]; // 决定下一个遍历节点
				begin[i] /= radix; // 数字缩减
				if (begin[i] != 0) { // 如果全部是0, 终止循环
					none_zero = true;
				}
			}
			int r = 0; // 找到第一个桶
			for (; r < radix; ++r) {
				if (len != rear[r]) {
					start = front[r];
					finish = rear[r];
					break;
				}
			}
			++r; // 把所有桶串接
			for (; r < radix; ++r) {
				if (rear[r] == len) { continue; }
				link[finish] = front[r];
				finish = rear[r];
			}
			link[finish] = len; // 把最后一个连接到无效值
			sg_fill(front, front + radix, len); // 把辅助数组连接到无效值
			sg_fill(rear, rear + radix, len);
		}
		auto i = start;
		for (auto j = 0; j != len; i = link[i], ++j) { // 以link的顺序，重新排列数组
			begin[j] = std::move(raw_array[i]);
		}
		sg_deallocate(front, radix); // 删除所有辅助数组
		sg_deallocate(rear, radix);
		sg_deallocate(link, len);
		sg_destroy(raw_array, raw_array + len);
		sg_deallocate(raw_array, len);
	}
	
	template <typename RandomIter, typename T, typename Comp>
	RandomIter sg_lower_bound(RandomIter begin, RandomIter end, const T& value, Comp comp) {
		size_t length = end - begin;
		size_t half_length;
		RandomIter middle_iter;
		
		while(length) {
			half_length = length / 2;
			middle_iter = begin + half_length;
			if (comp(*middle_iter, value)) {
				begin  = middle_iter + 1;
				length -= half_length + 1;
			} else {
				length = half_length;
			}
		}
		return begin;
	}
	
	template <typename ForwardIter, typename T>
	ForwardIter sg_lower_bound(ForwardIter begin, ForwardIter end, const T& value) {
		return sg_lower_bound(begin, end, value, std::less<>());
	}
}

#endif// SG_STL_SORTER_H

		
		
		
		
		
		
		
		
		
		
		
		
		
		
		
		
		
		
		
		
		
		
		
		
		
		
		
		
		
		
		
		
		
		
		
		
		
		
		
		
		
		
		
		
		
		
		
		
		
		
		
		
		
		
		
		
		
		
		
		
		
		
		
		
		
		
		
		
		
		
		
		
		
		
		
		
		
		
		
		
		
		
		
		
		
		
		
		
		
		
		
		
		
		
		
		
		
		
		
		
		
		
		
		
		
		
		
		
		
		
		
		
		
		
		
		
		
		
		
		
		
		
		
		
		
		
		
		
		
		
		
		
		
		
		
		
		
		
		
		
		
		
		
		
		
		
		
		
		
		
		
		
		
		
		
		
		
		
		
		
		
		
		
		
		
		
		
		
		
		
		
		
		
		
		
		
		
		
		
		
		
		
		
		
		
		
		
		
		
		
		
		
		
		
		
		
		
		
		
		
		
		
		
		
		
		
		
		
		
		
		
		
		
		
		
		
		
		
		
		
		
		
		
		
		
		
		
		
		
		
		
		
		
		
		
		
		
		
		
		
		
		
		
		
		
		
		
		
		
		
		
		
		
		
		
		
		
		
		
		
		
		
		
		
		
		
		
		
		
		
		
		
		
		
		
		
		
		

		
		

		
		
		
		
		
		
		
		
		
		
		
		
		
		
		
		
		
		
		
		
		
		
		
		
		
		
		
		
		
		
		
		
		
		
		
		
		
		
		
		
		
		
		
		
		
		
		
		
		
		
		
		
		
		
		
		
		
		
		
		
		
		
		
		
		
		
		
		
		
		
		
		
		
		
		
		
		
		
		
		
		
		
		
		
		
		
		
		
		
		
		
		
		
		
		
		
		
		
		
		
		
		
		
		
		
		
		
		
		
		
		
		
		
		
		
		
		
		
		
		
		
		
		
		
		
		
		
		
		
		
		
		
		
		
		
		
		
		
		
		
		
		
		
		
		
		
		
		
		
		
		
		
		
		
		
		
		
		
		
		
		
		
		
		
		
		
		
		
		
		
		
		
		
		
		
		
		
		
		
		
		
		
		
		
		
		
		
		
		
		
		
		
		
		
		
		
		
		
		
		
		
		
		
		
		
		
		
		
		
		
		
		
		
		
		
		
		
		
		
		
		
		
		
		
		
		
		
		
		
		
		
		
		
		
		
		
		
		
		
		
		
		
		
		
		
		
		
		
		
		
		
		
		
		
		
		
		
		
		
		
		
		
		
		
		
		
		
		
		
		
		
		
		
		
		
		
		
		
		
		
		
		
		
		
		
		
		
		
		
		
		
		
		
		
		
		
		
		
		
		
		
		
		
		
		
		
		
		
		
		
		
		
		
		
		
		
		
		
		
		
		
		
		
		
		
		
		
		
		
		
		
		
		
		
		
		
		
		
		
		
		
		
		
		
		
		
		
		
		
		
		
		
		
		
		
		
		
		
		
		
		
		
		
		
		
		
		
		
		
		
		
		
		
		
		
		
		
		
		
		
		
		
		
		
		
		
		
		
		
		
		
		
		
		
		
		
		
		
		
		
		
		
		
		
		
		
		
		
		
		
		
		
		
		
		
		
		
		
		
		
		
		
		
		
		
		
		
		
		
		
		
		
		
		
		
		
		
		
		
		
		
		
		
		
		
		
		
		
		
		
		
		
		
		
		
		
		
		
		
		
		
		
		
		
		
		
		
		
		
		
		
		
		
		
		
		
		
		
		
		
		
		
		
		
		
		
		
		
		
		
		
		
		
		
		
		
		
		
		
		
		
		
		
		
		
		
		
		
		
		
		
		
		
		
		
		
		
		
		
		
		
		
		
		
		
		
		
		
		
		
		
		
		
		
		
		
		
		
		
		
		
		
		
		
		
		
		
		
		
		
		
		
		
		
		
		
		
		
		
		
		
		
		
		
		
		
		
		
		
		
		
		
		
		
		
		
		
		
		
		
		
		
		
		
		
		
		
		
		
		
		
		
		
		
		
		
		
		
		
		
		
		
		
		
		
		
		
		
		
		
		
		
		
		
		
		
		
		
		
		
		
		
		
		
		
		
		
		
		
		
		
		
		
		
		
		
		
		
		
		
		
		
		
		
		
		
		
		
		
		
		
		
		
		
		
		
		
		
		
		
		
		
		
		
		
		
		
		
		
		
		
		
		
		
		
		
		
		
		
		
		
		
		
		
		
		
		
		
		
		
		
		
		
		
		
		
		
		
		
		
		
		
		
		
		
		
		
		
		
		
		
		
		
		
		
		
		
		
		
		
		
		
		
		
		
		
		
		
		
		
		
		
		
		
		
		
		
		
		
		
		
		
		
		
		
		
		
		
		
		
		
		
		
		
		
		
		
		
		
		
		
		
		
		
		
		
		
		
		
		
		
		
		
		
		
		
		
		
		
		
		
		
		
		
		
		
		
		
		
		
		
		
		
		
		
		
		
		
		
		
		
		
		
		
		
		
		
		
		
		
		
		
		
		
		
		
		
		
		
		
