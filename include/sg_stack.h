//
// Created by dell on 2019/9/25.
//

#ifndef SG_STL_SG_STACK_H
#define SG_STL_SG_STACK_H

#include "sg_vector.h"

namespace sg {
	template <typename T,typename Sequence = sg_vector<T> >
	class sg_stack {
		// 泛型相关代码
		using value_type = typename Sequence::value_type;
		using reference = typename Sequence::reference;
		using const_reference = typename Sequence::const_reference;

	protected:
		Sequence c; // 底层容器
	
	public:

		// 以下操作均直接转发给底层容器
		bool empty() const {
			return c.empty();
		}

		size_type size() const {
			return c.size();
		}

		const_reference top() const {
			return c.back();
		}

		void push(const T& value) {
			c.push_back(value);
		}

		void pop() {
			c.pop_back();
		}
	};
}
#endif //SG_STL_SG_STACK_H
