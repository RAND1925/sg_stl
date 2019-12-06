//
// Created by dell on 2019/12/3.
//

#ifndef SG_STL_SG_PRIORITY_QUEUE_H
#define SG_STL_SG_PRIORITY_QUEUE_H

namespace sg {
	template <typename RandomIter, typename T, typename Comp>
	constexpr void sg__heap_sift_up(RandomIter begin, size_t hole_index, size_t top_index, T&& value, Comp comp) {
		for (size_t parent_index = (hole_index - 1) / 2;
		     top_index < hole_index && comp(begin[parent_index], value);
		     parent_index = (hole_index - 1) / 2) {
			begin[hole_index] = std::move(begin[parent_index]);
			hole_index = parent_index;
		}
		begin[hole_index] = value;
	}
	template <typename RandomIter, typename Comp>
	constexpr void sg_push_heap(RandomIter begin, RandomIter end, Comp comp) {
		size_t length = end - begin;
		if (length >= 2) {
			auto value = std::move(*--end);
			sg__heap_sift_up(begin, length - 1, 0, value, comp);
		}
	}
	
	template <typename RandomIter, typename T, typename Comp>
	constexpr void sg__heap_sift_down(RandomIter begin, size_t hole_index,size_t bottom_index,T&& value, Comp comp) {
		const size_t last_non_leaf_index =  (bottom_index - 1) / 2;
		size_t child_index = hole_index;
		const size_t top_index = hole_index;
		while (child_index < last_non_leaf_index) {
			child_index = 2 * child_index + 2;
			if (comp(begin[child_index] , begin[child_index - 1])) {
				--child_index;
			}
			begin[hole_index] = std::move(begin[child_index]);
			hole_index = child_index;
		}
		if (child_index == bottom_index && bottom_index % 2 == 0) {
			--child_index;
			begin[hole_index] = std::move(begin[child_index]);
			hole_index = child_index;
		}
		sg__heap_sift_up(begin, hole_index, top_index, value, comp);
	}
	template <typename RandomIter, typename Comp>
	constexpr void sg_pop_heap(RandomIter begin, RandomIter end, Comp comp) {
		size_t length = end - begin;
		if (length >= 2) {
			auto value= std::move(*--end);
			*end = std::move(*begin);
			sg__heap_sift_down(begin, 0, length - 1, value, comp);
		}
	}
	
	template <typename RandomIter, typename Comp>
	constexpr void sg_make_heap(RandomIter begin, RandomIter end, Comp comp) {
		size_t length = end - begin;
		if (length > 2) {
			for (size_t hole_index = length / 2 ; hole_index > 0;) {
				--hole_index;
				auto value = std::move(begin[hole_index]);
				sg__heap_sift_down(begin, hole_index, length, std::move(value), comp);
			}
		}
	}
	
	template <typename RandomIter>
	constexpr void sg_push_heap(RandomIter begin, RandomIter end) {
		sg_push_heap(begin, end, std::less<>());
	}
	template <typename RandomIter>
	constexpr void sg_pop_heap(RandomIter begin, RandomIter end) {
		sg_pop_heap(begin, end, std::less<>());
	}
	template <typename RandomIter>
	constexpr void sg_make_heap(RandomIter begin, RandomIter end) {
		sg_make_heap(begin, end, std::less<>());
	}
	
	template <typename T, typename Comp = std::less<T>, typename Sequence = std::vector<T> >
	class sg_priority_queue {
	private:
		Sequence c;
		Comp comp;
	public:
		sg_priority_queue(): c(){}
		explicit sg_priority_queue(const Comp& comp): comp(comp), c(){}
		using const_reference = const T&;
		using value_type = T;
		bool empty() const { return c.empty();}
		size_t size() const { return c.size();}
		const_reference top() const { return c.front();}
		void push(const T& value) {
			c.push_back(value);
			sg_push_heap(c.begin(), c.end(), comp);
		}
		void pop() {
			sg_pop_heap(c.begin(), c.end(), comp);
			c.pop_back();
		}
	};
}
#endif //SG_STL_SG_PRIORITY_QUEUE_H
