//
// Created by dell on 2019/12/3.
//

#ifndef SG_STL_SG_PRIORITY_QUEUE_H
#define SG_STL_SG_PRIORITY_QUEUE_H

#include "sg_vector.h"
#include "__algorithm/sg_algo_heap.h"
namespace sg {
	
	template <typename T, typename Comp = std::less<T>, typename Container = sg::sg_vector<T> >
	class sg_priority_queue {
	public:
		//泛型相关算法
		using const_reference = const T &;
		using value_type = T;
	protected:
		
		Comp comp; // 存储比较用函数
		Container c; // 存储底层容器

	public:
		// 转发给底层容器
		bool empty() const { return c.empty(); }
		size_t size() const { return c.size(); }

		const_reference top() const { return c.front(); } 

		sg_priority_queue(): c(){}
		explicit sg_priority_queue(const Comp& comp): comp(comp), c(){}
		// 构造函数，从其他容器内复制，调用算法构造成堆
		sg_priority_queue(const Comp& comp, const Container& cont): comp(comp), c(cont){ 
			sg_make_heap(c.begin(), c.end(), comp);
		}
		sg_priority_queue(const Comp& comp, Container&& cont): comp(comp), c(cont){
			sg_make_heap(c.begin(), c.end(), comp);
		}
		
		// 将新数据压入到内部容器尾部，调用 sift up 调整成堆
		void push(const T& value) {
			c.push_back(value);
			sg_push_heap(c.begin(), c.end(), comp);
		}

		// 将头部与尾部数据交换，调用 sift down 调整成堆，并且移出内存
		void pop() {
			sg_pop_heap(c.begin(), c.end(), comp);
			c.pop_back();
		}
	};
}
#endif //SG_STL_SG_PRIORITY_QUEUE_H
