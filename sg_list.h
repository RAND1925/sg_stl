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
		//每个节点有一个对应数据和前后指针
		using pointer = sg_list_node<T> *;
		pointer next = nullptr;
		pointer prev = nullptr;
		T data;
	};

	template <typename T>
	struct sg_list_iterator{
		// 泛型相关代码
		using self = sg_list_iterator<T>;
		using iterator_category = sg_bidirectional_iterator_tag;
		using value_type = T;
		using pointer = T*;
		using reference = T&;
		using link_type = sg_list_node<T>*;
		
		//构造函数
		sg_list_iterator() = default;
		sg_list_iterator(link_type x): node(x){}
		sg_list_iterator(const sg_list_iterator<T>& x): node(x.node){}
		
		// 一个对应节点的指针
		link_type node = nullptr;
		
		// 转发指针代码
		reference operator* () const;
		pointer& operator-> () const;

		// 将节点的前后移操作映射到 ++/--，使得其可以直接使用指针代码
		sg_list_iterator& operator++();
		const sg_list_iterator operator++(int);
		sg_list_iterator& operator--();
		const sg_list_iterator operator--(int);
		bool operator==(const self& rhs) const;
		bool operator!=(const self& rhs) const;
	};
	
	template <typename T>
	typename sg_list_iterator<T>::reference sg_list_iterator<T>::operator*() const {
		return node->data;
	}
	
	template <typename T>
	typename sg_list_iterator<T>::pointer& sg_list_iterator<T>::operator->() const {
		return &(node->data);
	}
	
	template <typename T>
	sg_list_iterator<T>& sg_list_iterator<T>::operator++() {
		node = (link_type)((node->next));
		return *this;
	}
	
	template <typename T>
	sg_list_iterator<T>& sg_list_iterator<T>::operator--() {
		node = (link_type)((node->prev));
		return *this;
	}
	
	template <typename T>
	const sg_list_iterator<T> sg_list_iterator<T>::operator++(int) {
		auto temp = *this;
		++(*this);
		return temp;
	}
	
	template <typename T>
	const sg_list_iterator<T> sg_list_iterator<T>::operator--(int) {
		auto temp = *this;
		--(*this);
		return temp;
	}
	
	template <typename T>
	bool sg_list_iterator<T>::operator==(const sg_list_iterator::self& rhs) const {
		return node == rhs.node;
	}
	
	template <typename T>
	bool sg_list_iterator<T>::operator!=(const sg_list_iterator::self& rhs) const {
		return !(*this == rhs);
	}
	
	
	template <typename T>
	class sg_list {
	public:
		//泛型相关指令
		using value_type = T;
		using reference = T &;
		using pointer = T *;
		using const_reference = const T &;
		using const_pointer = const T *;
		using list_node = sg_list_node<T>;
		using link_type = list_node *;
		using iterator = sg_list_iterator<T>;

	protected:
		link_type header; // 头节点
		
		link_type create_node(const_reference x); // 创建节点
		void destroy_node(link_type p); // 删除节点

	public:
		iterator begin() const; // 返回其实迭代器，即 header->next
		iterator end() const; // 返回尾迭代器，即header
		const_reference front() const; // 返回有效的第一个有效节点的值
		const_reference back() const; // 返回链表的最后一个有效节点的值
		[[nodiscard]] bool empty() const; // 通过header是否链接自身返回链表是否为空
		[[nodiscard]] size_t size() const; // 通过步进返回链表大小

		sg_list(); // 创建头节点header，两个指针自指
		void clear(); // 清空
		~sg_list(); // 调用clear，并删除header
		
		iterator insert(iterator position, const_reference data); // 向 position 之前插入，返回被插入节点
		iterator erase(iterator position); // 删除所指节点，返回后一个节点
		iterator push_back(const_reference data); // 即insert(end(), data)
		iterator push_front(const_reference data); // 即insert(begin(), data)
		iterator pop_back(); // 即erase(prev(end()))
		iterator pop_front(); // 即erase(begin())
	};
	
	template <typename T>
	typename sg_list<T>::link_type sg_list<T>::create_node(const_reference x) {
		link_type p = sg_allocate<sg_list<T>::list_node>(1);
		sg_construct(&(p->data), x);
		return p;
	}
	
	template <typename T>
	void sg_list<T>::destroy_node(sg_list::link_type p) {
		p->data.~T();
		sg_deallocate(p, 1);
	}
	
	template <typename T>
	typename sg_list<T>::iterator sg_list<T>::begin() const {
		return (link_type) (header->next);
	}
	
	template <typename T>
	typename sg_list<T>::iterator sg_list<T>::end() const {
		return (link_type) header;
	}
	
	template <typename T>
	typename sg_list<T>::const_reference sg_list<T>::front() const {
		return header->next->data;
	}
	
	template <typename T>
	typename sg_list<T>::const_reference sg_list<T>::back() const {
		return header->prev->data;
	}
	
	template <typename T>
	bool sg_list<T>::empty() const {
		return header->next == header;
	}
	
	template <typename T>
	size_t sg_list<T>::size() const {
		size_t count = 0;
		for (auto iter = begin(); iter != end(); ++iter) {
			count++;
		}
		return count;
	}
	
	template <typename T>
	sg_list<T>::sg_list() {
		header = sg_allocate<list_node>(1);
		header->next = header;
		header->prev = header;
	}
	
	template <typename T>
	typename sg_list<T>::iterator sg_list<T>::insert(sg_list::iterator position, const_reference data) {
		link_type tmp = create_node(data); // 创建节点
		tmp->next = position.node; // 修改新节点的前后指针
		tmp->prev = position.node->prev;
		position.node->prev->next = tmp; // 修改原有节点的前后指针
		position.node->prev = tmp;
		return tmp; // 返回新节点
	}
	
	template <typename T>
	typename sg_list<T>::iterator sg_list<T>::erase(sg_list::iterator position) {
		link_type next_node = position.node->next; // 记录原来节点的前后节点
		link_type prev_node = position.node->prev;
		next_node->prev = prev_node; // 将他们直接相连
		prev_node->next = next_node;
		destroy_node(position.node); // 删除指定节点
		return {next_node}; // 返回所指的节点的后继节点
	}
	
	template <typename T>
	typename sg_list<T>::iterator sg_list<T>::pop_front() {
		return erase(begin());
	}
	
	template <typename T>
	typename sg_list<T>::iterator sg_list<T>::pop_back() {
		auto last = end();
		return erase(--last);
	}
	
	template <typename T>
	typename sg_list<T>::iterator sg_list<T>::push_back(const_reference data) {
		return insert(end(), data);
	}
	
	template <typename T>
	typename sg_list<T>::iterator sg_list<T>::push_front(const_reference data) {
		return insert(begin(), data);
	}
	
	template <typename T>
	void sg_list<T>::clear() {
		link_type cur = header->next; // 从首节点开始
		while (cur != header) { // 依次删除所有节点
			link_type tmp = cur;
			cur = cur->next;
			destroy_node(tmp);
		}
		header->next = header; // 头尾指针重新指向自己
		header->prev = header;
	}
	
	template <typename T>
	sg_list<T>::~sg_list() {
		clear(); // 依次删除所有节点
		destroy_node(header); // 然后删除头结点
	}
	
}

#endif //SG_STL_SG_LIST_H
