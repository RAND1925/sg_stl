//
// Created by dell on 2019/9/25.
//

#ifndef SG_STL_SG_LIST_H
#define SG_STL_SG_LIST_H

#include "sg_basic.h"
#include "sg_allocator.h"
namespace sg {
	template <typename T>
	struct sg_list_node {
		using pointer = sg_list_node<T> *;
		pointer next = nullptr;
		pointer prev = nullptr;
		T data;
	};


	template <typename T, typename Reference, typename Pointer>
	class sg_list_iterator{
	public:
		using self = sg_list_iterator<T, Reference, Pointer>;
		using iterator = sg_list_iterator<T, T& ,T*>;
		using size_type = size_t;
		using iterator_category = sg_bidirectional_iterator_tag;
		using value_type = T;
		using pointer = Pointer;
		using reference = Reference;
		using link_type = sg_list_node<T>*;


		//constructor
		sg_list_iterator() = default;
		sg_list_iterator(link_type x): node(x){}
		sg_list_iterator(const sg_list_iterator<T, Reference, Pointer>& x): node(x.node){}
		reference operator* () const {
			return node->data;
		}
		pointer operator-> () const {
			return &(node->data);
		}

		self& operator++() {
			node = (link_type)((node->next));
			return *this;
		}

		const self operator++(int){
			self temp = *this;
			++(*this);
			return temp;
		}

		self& operator--() {
			node = (link_type)((node->prev));
			return *this;
		}

		const self operator--(int){
			self temp = *this;
			--(*this);
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
	using sg_list_node_allocator = std::allocator< sg_list_node<T> >();

	template <typename T, typename Allocator = std::allocator<T> >
	class sg_list {
		//cycle list
	public:
		using value_type = T;
		using reference = T &;
		using pointer = T *;
		using const_reference = const T &;
		using const_pointer = const T *;
		using list_node = sg_list_node<T>;
		using link_type = list_node *;
		using iterator = sg_list_iterator<T, reference, pointer>;

	protected:
		link_type header;
		size_type m_size = 0;
		link_type alloc_node() {
			return new list_node();
		}

		void dealloc_node(link_type p) {
			delete p;
		}

		link_type create_node(const_reference x) {
			link_type p = alloc_node();
			p->data = x;
			return p;
		}

		void destroy_node(link_type p) {
			dealloc_node(p);
		}

	public:
		iterator begin() const  {
			return (link_type) (header->next);
		}

		iterator end() const {
			return (link_type) header;
		}

		const_reference front() const {
			return *begin();
		}

		const_reference back() const {
			return *(--end());
		}

		bool empty() const {
			return header->next == header;
		}

		size_type size() const {
			return m_size;
		}

		sg_list() {
			header = alloc_node();
			header->next = header;
			header->prev = header;
		}

		iterator insert(iterator position, const_reference data) {
			link_type tmp = create_node(data);
			tmp->next = position.node;
			tmp->prev = position.node->prev;
			position.node->prev->next = tmp;
			position.node->prev = tmp;
			++m_size;
			return tmp;
		}

		iterator push_back(const_reference data) {
			return insert(end(), data);
		}

		iterator push_front(const_reference data) {
			return insert(begin(), data);
		}

		iterator erase(iterator position) {
			link_type next_node = position.node->next;
			link_type prev_node = position.node->prev;
			next_node->prev = prev_node;
			prev_node->next = next_node;
			destroy_node(position.node);
			--m_size;
			return {next_node};
		}

		iterator pop_front() {
			return erase(begin());
		}

		iterator pop_back() {
			return erase(--end());
		}

	};

}

#endif //SG_STL_SG_LIST_H
