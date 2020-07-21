//
// Created by dell on 2019/9/27.
//

#ifndef SG_STL_SG_QUEUE_H
#define SG_STL_SG_QUEUE_H


//
// Created by dell on 2019/9/25.
//

#include "sg_list.h"
namespace sg {
	template <typename T, typename Sequence = sg_list<T> >
	class sg_queue {
	public:

		// 泛型相关代码
		using value_type = typename Sequence::value_type;
		using reference = typename Sequence::reference;
		using const_reference = typename Sequence::const_reference;

	protected:
		Sequence c; // 底层容器

	public:

		// 直接转发给底层容器
		bool empty() const {
			return c.empty();
		}

		size_type size() const {
			return c.size();
		}

		const_reference front() const {
			return c.front();
		}

		void push(const T& data) {
			c.push_back(data);
		}

		void pop() {
			c.pop_front();
		}
	};

}
#endif //SG_STL_SG_QUEUE_H
