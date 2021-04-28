//
// Created by dell on 2019/12/22.
//

#ifndef SG_STL_SG_ALGO_HEAP_H
#define SG_STL_SG_ALGO_HEAP_H



#include <functional>

template <typename RandomIter, typename Comp>
constexpr RandomIter sg_is_heap_until(RandomIter begin, RandomIter end, Comp comp) {
	size_t len = end - begin;
	if (len > 1) {
		// 遍历到一半的位置
		for (size_t i = 0; ++i < len;) {
			// 判断节点是否小于他的子节点
			if (comp(begin[(i - 1) / 2], begin[i]))
				return begin + i;
		}
	}
	return end;
}

template <typename RandomIter, typename Comp>
constexpr bool sg_is_heap(RandomIter begin, RandomIter end, Comp comp) {
	return sg_is_heap_until(begin, end, comp) == end;
}

template <typename RandomIter>
constexpr RandomIter sg_is_heap_until(RandomIter begin, RandomIter end) {
	return sg_is_heap_until(begin, end, std::less<>());
}

template <typename RandomIter>
constexpr bool sg_is_heap(RandomIter begin, RandomIter end) {
	return sg_is_heap(begin, end, std::less<>());
}


template <typename RandomIter, typename T, typename Comp>
constexpr void _sg_heap_sift_up(RandomIter begin, size_t hole_index, size_t top_index, T&& value, Comp comp) {
	//找到“洞”的父节点
	for (size_t parent_index = (hole_index - 1) / 2;
		 // 直到堆顶，或者父节点比“值”优先，结束循环
	     top_index < hole_index && comp(begin[parent_index], value);
	     parent_index = (hole_index - 1) / 2) {
		// “洞”父节点的值，更新“洞”
		begin[hole_index] = std::move(begin[parent_index]);
		hole_index = parent_index;
	}
	// 将“值”放入“洞”中
	begin[hole_index] = std::move(value);
}

template <typename RandomIter, typename Comp>
constexpr void sg_push_heap(RandomIter begin, RandomIter end, Comp comp) {
	size_t length = end - begin;
	if (length > 1) {
		// 首先把最后一个保存在临时变量“值”里，此时尾部为“洞”
		auto value = std::move(*--end);
		_sg_heap_sift_up(begin, length - 1, 0, value, comp);
	}
}





template <typename RandomIter>
constexpr void sg_push_heap(RandomIter begin, RandomIter end) {
	sg_push_heap(begin, end, std::less<>());
}

template <typename RandomIter, typename T, typename Comp>
constexpr void _sg_heap_sift_down(RandomIter begin, size_t hole_index, size_t bottom_index, T&& value, Comp comp) {
	//找到最后一个满节点之后的位置
	const size_t last_max_sequence_after = (bottom_index - 1) / 2;
	const size_t top_index = hole_index;
	
	size_t child_index = hole_index;
	while (hole_index < last_max_sequence_after) {
		// 找到两个子节点中大的那个
		child_index = 2 * hole_index + 2;
		if (comp(begin[child_index] , begin[child_index - 1])) {
			--child_index;
		}
		// 将“子节点”的数据移入“洞”，更新“洞”
		begin[hole_index] = std::move(begin[child_index]);
		hole_index = child_index;
	}
	// 如果“洞”落到最后一个满节点之后，需要特殊处理
	if (hole_index == last_max_sequence_after && bottom_index % 2 == 0) {
		begin[hole_index] = std::move(begin[bottom_index - 1]);
		hole_index = bottom_index - 1;
	}
	// 随后，“洞”必然落到堆底，再据此调用sift up 重新插入堆
	_sg_heap_sift_up(begin, hole_index, top_index, value, comp);
}

template <typename RandomIter, typename Comp>
constexpr void sg_pop_heap(RandomIter begin, RandomIter end, Comp comp) {
	size_t length = end - begin;
	if (length > 1) {
		// 首元素移入临时变量，尾元素移入首位置
		auto value= std::move(*--end);
		*end = std::move(*begin);
		// 调用sift down 调整，其中尾的位置为“洞”
		_sg_heap_sift_down(begin, 0, length - 1, value, comp);
	}
}

template <typename RandomIter>
constexpr void sg_pop_heap(RandomIter begin, RandomIter end) {
	sg_pop_heap(begin, end, std::less<>());
}


template <typename RandomIter, typename Comp>
constexpr void sg_make_heap(RandomIter begin, RandomIter end, Comp comp) {
	size_t length = end - begin;
	if (length > 1) {
		// “洞”初始化为中点
		for (size_t hole_index = length / 2; hole_index > 0;) {
			--hole_index;
			// 将“洞”移入临时变量
			auto value =std::move(begin[hole_index]);
			// 用sift down调整堆
			_sg_heap_sift_down(begin, hole_index, length, std::move(value), comp);
		}
	}
}

template <typename RandomIter>
constexpr void sg_make_heap(RandomIter begin, RandomIter end) {
	sg_make_heap(begin, end, std::less<>());
}

template <typename RandomIter, typename Comp>
constexpr void sg_sort_heap(RandomIter begin, RandomIter end, Comp comp) {
	while (end - begin > 1) {
		sg_pop_heap(begin, end, comp);
		--end;
	}
}

template <typename RandomIter>
constexpr void sg_sort_heap(RandomIter begin, RandomIter end) {
	sg_sort_heap(begin, end, std::less<>());
}


#endif //SG_STL_SG_ALGO_HEAP_H
