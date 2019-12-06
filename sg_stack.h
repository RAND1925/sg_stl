//
// Created by dell on 2019/9/25.
//

#ifndef SG_STL_SG_STACK_H
#define SG_STL_SG_STACK_H

#include "sg_list.h"
namespace sg {
	template <typename T,typename Sequence = sg_list<T> >
	class sg_stack {
		using value_type = typename Sequence::value_type;
		using reference = typename Sequence::reference;
		using const_reference = typename Sequence::const_reference;

	protected:
		Sequence c;

		bool empty() const {
			return c.empty();
		}

		size_type size() const {
			return c.size();
		}

		const_reference top() const {
			return c.back();
		}

		void push() {
			c.push_back();
		}

		void pop() {
			c.pop_back();
		}
	};

}
#endif //SG_STL_SG_STACK_H
