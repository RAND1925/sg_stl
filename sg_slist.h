//
// Created by dell on 2019/9/18.
//

#ifndef SG_STL_SG_SLIST_H
#define SG_STL_SG_SLIST_H

#include "sg_allocator.h"
namespace sg {
	template <typename T>
	struct sg_slist_node {
		using pointer = sg_slist_node<T> *;
		pointer next;
		T data;
	};


	template <typename T>
	class sg_slist_iterator{
	public:
		using self = sg_slist_iterator<T>;
		using iterator_category = sg_forward_iterator_tag;
		using value_type = T;
		using pointer = T*;
		using reference = T&;
		using link_type = sg_slist_node<T>*;


		//constructor
		sg_slist_iterator() = default;
		sg_slist_iterator(link_type x): node(x){}
		sg_slist_iterator(const sg_slist_iterator<T>& x): node(x.node){}

		reference operator* () const{
			return node->data;
		}
		pointer operator-> () const{
			return &(node->data);
		}
		self& operator++() {
			node = node->next;
			return *this;
		}

		const self operator++(int){
			self temp = *this;
			++(*this);
			return temp;
		}

		bool operator==(const self& rhs){
			return node == rhs.node;
		}

		bool operator!=(const self& rhs){
			return !(*this == rhs);
		}

		link_type node = nullptr;


	};

	template <typename T>
	using sg_slist_node_allocator = std::allocator< sg_slist_node<T> >();

	template <typename T, typename Allocator = std::allocator<T> >
	class sg_slist {
		//cycle list
	public:
		using reference = T&;
		using pointer = T*;
		using const_reference = const T&;
		using const_pointer = const T*;
		using slist_node = sg_slist_node<T>;
		using link_type = slist_node*;
		using iterator = sg_slist_iterator<T>;
		using const_iterator = const sg_slist_iterator<T>;
		using node_allocator = sg::sg_allocator<slist_node>;

	protected:
		link_type header;

		link_type create_node() {
			return node_allocator::allocate();
		}

		link_type create_node(const_reference x) {
			link_type p = create_node();
			p->next = nullptr;
			construct(&(p->data), x);
			return p;
		}

		void destroy_node(link_type p) {
			deallocate(p);
		}

		iterator insert_after_base(const_iterator pos, const_reference data) {
			auto new_node = create_node(data);
			new_node->next = pos.node->next;
			pos.node->next = new_node;
			return new_node;
		}

		iterator erase_after_base(const_iterator pos) {
			auto removed_node = pos.node->next;
			pos.node->next = removed_node->next;
			destroy_node(removed_node);
			return pos.node->next;
		}

	public:

		template <typename Func>
		const_iterator find_before_if(Func func) const {
			auto iter = before_begin();
			for (;; iter++) {
				auto before = iter++;
				if (iter == end() || func(*iter)) {
					return iter;
				}
			}
		}

		void clear(){
			for (auto iter = before_begin(); iter.node->next == header;++iter) {
				erase_after(iter);

			}
		}

		iterator begin() const{
			return header->next;
		}

		iterator end() const {
			return header;
		}

		iterator before_begin() const {
			return end();
		}

		bool empty() const {
			return header->next == header;
		}

		size_type size() const {
			int siz = 0;
			for (auto iter = begin(); iter != end(); ++iter) {
				++siz;
			}
			return siz;
		}

		sg_slist() {
			header = create_node();
			header->next = header;
		}

		~sg_slist(){
			clear();
		}


		iterator insert_after(const_iterator pos, const_reference data) {
			return insert_after_base(pos, data);
		}
		iterator push_front (T &&data) {
			return insert_after_base(before_begin(), data);
		}

		iterator erase_after(const_iterator pos) {
			return erase_after_base(pos);
		}

		template <typename Func>
		size_t remove_if (Func func) {
			auto before = before_begin();
			size_t removed = 0;
			auto curr = begin();
			while (curr != end()){
				if (func(*curr)) {
					curr.node = erase_after_base(before).node;
					++removed;
				} else {
					before = curr;
					++before;
				}
			}
			return removed;
		}

		size_t remove(const T &value){
			return remove_if([this, &value](const T &v){return value == v;});
		}
	};

}


#endif //SG_STL_SG_SLIST_H
