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

		// 泛型相关代码
		using iterator_category = sg_forward_iterator_tag;
		using value_type = T;
		using pointer = T*;
		using reference = T&;
		using link_type = sg_slist_node<T>*;
		
		
		link_type node = nullptr;// 唯一的数据成员一个对应节点的指针
		
		// 构造函数
		sg_slist_iterator() = default;
		sg_slist_iterator(link_type x): node(x){}
		sg_slist_iterator(const sg_slist_iterator<T>& x): node(x.node){}
		
		reference operator* () const; // 转发到指针
		pointer operator-> () const;
		bool operator==(const sg_slist_iterator& rhs);
		bool operator!=(const sg_slist_iterator& rhs);
		
		sg_slist_iterator& operator++(); // 将++操作映射到node = node->next
		const sg_slist_iterator operator++(int);
	};
	
	template <typename T>
	typename sg_slist_iterator<T>::reference sg_slist_iterator<T>::operator*() const {
		return node->data;
	}
	
	template <typename T>
	typename sg_slist_iterator<T>::pointer sg_slist_iterator<T>::operator->() const {
		return &(node->data);
	}
	
	template <typename T>
	sg_slist_iterator<T>& sg_slist_iterator<T>::operator++() {
		node = node->next;
		return *this;
	}
	
	template <typename T>
	const sg_slist_iterator<T> sg_slist_iterator<T>::operator++(int) {
		auto temp = *this;
		++(*this);
		return temp;
	}
	
	template <typename T>
	bool sg_slist_iterator<T>::operator==(const sg_slist_iterator<T>& rhs) {
		return node == rhs.node;
	}
	
	template <typename T>
	bool sg_slist_iterator<T>::operator!=(const  sg_slist_iterator<T>& rhs) {
		return !(*this == rhs);
	}
	

	template <typename T>
	class sg_slist {

	public:
		// 泛型相关代码
		using reference = T&;
		using pointer = T*;
		using const_reference = const T&;
		using const_pointer = const T*;
		using slist_node = sg_slist_node<T>;
		using link_type = slist_node*;
		using iterator = sg_slist_iterator<T>;
		using const_iterator = const sg_slist_iterator<T>;

	protected:
		link_type header; // 唯一的数据成员：头节点
		
		link_type create_node(const_reference x); // 创造节点
		void destroy_node(link_type p); // 删除节点
	public:
		iterator begin() const; // 返回首个有效节点，即header->next
		iterator end() const; // 返回尾后节点，即header
		iterator before_begin() const; // 返回首个之前的节点，对于循环链表，也是header

		bool empty() const; // 通过 header->next == header 判断节点是否空
		size_type size() const;
		
		sg_slist(); // 构造函数，创建头结点并使它自指
		~sg_slist(); // 析构函数，清空，并删除头节点
		
		iterator insert_after(iterator pos, const_reference data); // 在节点之后插入
		iterator erase_after(iterator pos);// 在节点之后删除
		void clear(); // 清空
		
		template <typename Pred>
		const_iterator find_before_if(Pred pred) const;
		
		template <typename Func>
		size_t remove_if (Func func); // 删除满足条件的节点
		size_t remove(const T &value);
	};
	
	template <typename T>
	typename sg_slist<T>::link_type sg_slist<T>::create_node(const_reference x) {
		link_type p = sg_allocate<slist_node>();
		p->next = nullptr;
		sg::sg_construct(&(p->data), x);
		return p;
	}
	
	template <typename T>
	typename sg_slist<T>::iterator sg_slist<T>::erase_after(typename sg_slist<T>::iterator pos) {
		auto removed_node = pos.node->next;
		pos.node->next = removed_node->next;
		destroy_node(removed_node);
		return pos.node->next;
	}
	
	template <typename T>
	typename sg_slist<T>::iterator sg_slist<T>::before_begin() const {
		return header;
	}
	
	template <typename T>
	typename sg_slist<T>::iterator sg_slist<T>::end() const {
		return header;
	}
	
	template <typename T>
	sg_slist<T>::sg_slist() {
		header = sg_allocate<slist_node>(1);
		header->next = header;
	}
	
	template <typename T>
	typename sg_slist<T>::iterator sg_slist<T>::insert_after(typename sg_slist<T>::iterator pos, const_reference data) {
		auto new_node = create_node(data);
		new_node->next = pos.node->next;
		pos.node->next = new_node;
		return new_node;
	}
	
	template <typename T>
	void sg_slist<T>::destroy_node(typename sg_slist<T>::link_type p) {
		sg_destroy(&(p->data));
		sg_deallocate(p);
	}
	
	template <typename T>
	template <typename Pred>
	typename sg_slist<T>::const_iterator sg_slist<T>::find_before_if(Pred pred) const {
		link_type before_curr = header;
		
		while (before_curr->next != header) {
			if (pred(before_curr->next->data)) {
				return before_curr;
			}
			before_curr = before_curr->next;
		}
		return before_curr;
	}
	
	template <typename T>
	void sg_slist<T>::clear() { // 清空
		link_type cur = header->next; // 从首节点开始
		while (cur != header) { // 依次删除所有节点
			link_type tmp = cur;
			cur = cur->next;
			destroy_node(tmp);
		}
		header->next = header; // 后继指针指向自己
	}
	
	template <typename T>
	typename sg_slist<T>::iterator sg_slist<T>::begin() const {
		return header->next;
	}
	
	template <typename T>
	template <typename Func>
	size_t sg_slist<T>::remove_if(Func func) {
		auto before = before_begin();
		size_t removed = 0;
		auto curr = begin();
		while (curr != end()){
			if (func(*curr)) {
				curr.node = erase_after(before).node;
				++removed;
			} else {
				before = curr;
				++before;
			}
		}
		return removed;
	}
	
	template <typename T>
	bool sg_slist<T>::empty() const {
		return header->next == header;
	}
	
	template <typename T>
	size_type sg_slist<T>::size() const {
		size_t siz = 0;
		for (auto iter = begin(); iter != end(); ++iter) {
			++siz;
		}
		return siz;
	}
	
	template <typename T>
	sg_slist<T>::~sg_slist() {
		clear();
		sg_destroy(header);
	}
	
	template <typename T>
	size_t sg_slist<T>::remove(const T& value) {
		return remove_if([this, &value](const T &v){return value == v;});
	}
	
}


#endif //SG_STL_SG_SLIST_H
