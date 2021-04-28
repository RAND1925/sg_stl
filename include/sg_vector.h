//
// Created by dell on 2019/9/18.
//

#ifndef SG_STL_SG_VECTOR_H
#define SG_STL_SG_VECTOR_H

#include "sg_basic.h"
#include "__memory/sg_allocator.h"
#include "__algorithm/sg_algo_base.h"
namespace sg {
	
	template<typename T>
	class sg_vector {
	public:
		// 泛型相关代码
		using value_type = T;
		using reference = T&;
		using pointer = T*;
		using const_reference = const T&;
		using const_pointer = const T*;

		// sg_vector 直接以指针为迭代器
		using iterator = pointer;
		using const_iterator = const_pointer;
		using iterator_category = sg_random_access_iterator_tag;

	protected:

		iterator start = nullptr; // 数据起点
		iterator finish = nullptr;  // 有效数据终点
		iterator end_of_storage = nullptr; // 可用内存终点
		void initialize_fill_n_aux_(const_reference data, size_t n = 1); // 初始化辅助
		void insert_aux_(iterator pos, const_reference data); // 插入辅助
		
	public:

		// 各构造函数
		sg_vector() = default;
		explicit sg_vector(size_t n);
		sg_vector(size_t n, const T& value);
		sg_vector(const sg_vector& other);
		sg_vector(sg_vector&& other);
		template <typename RandomIter>
		sg_vector(RandomIter begin, RandomIter end) {
			reserve(end - begin);
			finish = sg_uninitialized_copy(begin, end, start);
		}
		~sg_vector();
		
		// 各个重新调整大小的函数
		void resize(size_t n);
		void resize(size_t n, const T& value);
		void reserve(size_t n);
		
		// 可以如数组一样访问的函数
		iterator begin() const { return start; }
		iterator end() const { return finish; }
		size_t size() const { return finish - start; }
		size_t capacity() const { return end_of_storage - start; }
		bool empty() const { return start == finish; }
		const_reference front() const { return *start; }
		const_reference back() const { return *(finish - 1); }
		reference operator[] (size_t n) const {	return *(start + n); }
		
		// 尾部增删
		void push_back(T&& data);
		void push_back(const_reference data);
		void pop_back();
		
		// 中部增删
		iterator insert(iterator pos, T&& data);
		iterator insert(iterator pos, const_reference data);
		iterator erase(iterator pos);
		iterator erase(iterator begin, iterator end);
		
		// 清空
		void clear();
	};
	
	template <typename T>
	void sg_vector<T>::push_back(T&& data) {
		if (finish != end_of_storage) { 
			sg_construct<T>(finish, data);
			++finish;
		} else {
			insert_aux_(finish, data);
		}
	}
	
	
	
	template <typename T>
	void sg_vector<T>::push_back(const_reference data) {
		if (finish != end_of_storage) { // 如果仍然有空间
			sg_construct<T>(finish, data); // 则直接在最后构建
			++finish; // 并且调整finish位置
		}
		else {
			insert_aux_(finish, data); // 否则，重新分配内存
		}
	}
	
	template <typename T>
	void sg_vector<T>::reserve(size_t n) {
		if (n > capacity()) {
			iterator new_start = sg_allocate<T>(n);
			iterator new_finish = sg_uninitialized_move(start, finish, new_start);
			sg_destroy(start, finish);
			sg_deallocate(start, capacity());
			start = new_start;
			finish = new_finish;
			end_of_storage = start + n;
		}
	}
	
	template <typename T>
	void sg_vector<T>::resize(size_t n, const T& value) {
		if (n == size()) {
			return;
		}
		iterator new_start = sg_allocate<T>(n);
		iterator new_finish;
		if (n < size()) {
			new_finish = sg_uninitialized_move(start, start + n, new_start);
		}
		else {
			new_finish = sg_uninitialized_move(start, finish, new_start);
			new_finish = sg_uninitialized_fill_n(new_finish, n - size(), value);
		}
		sg_destroy(start, finish);
		sg_deallocate(start, capacity());
		start = new_start;
		finish = new_finish;
		end_of_storage = finish;
	}
	
	template <typename T>
	void sg_vector<T>::resize(size_t n) {
		resize(n, T());
	}
	
	template <typename T>
	void sg_vector<T>::initialize_fill_n_aux_(const_reference data, size_t n) {
		iterator new_start = nullptr;
		try {
			new_start = sg_allocate<T>(n);
		} catch (sg_out_of_memory_exception& e){
			throw;
		}
		sg_uninitialized_fill_n(new_start, n, data);
		start = new_start;
		finish = new_start + n;
		end_of_storage = finish;
	}
	
	template <typename T>
	void sg_vector<T>::insert_aux_(typename sg_vector<T>::iterator pos, const_reference data) {
		const size_t old_size = size();
		const size_t new_capacity = old_size ? 2 * old_size : 1; // 内存扩容为两倍
		iterator new_start = nullptr;
		try {
			new_start = sg_allocate<T>(new_capacity); // 申请两倍大小的内存
		} catch (sg_out_of_memory_exception& e){
			throw;
		}
		iterator new_finish = sg_uninitialized_move(start, pos, new_start); // 将前半移动到新内存
		sg_construct(new_finish, data); // 在pos位置插入对象
		++new_finish;
		new_finish = sg_uninitialized_move(pos, finish, new_finish); // 将后半移动到新内存
		sg_destroy(start, finish); // 删掉原来的内存
		sg_deallocate(start, capacity());
		start = new_start; // 重置指针
		finish = new_finish;
		end_of_storage = start + new_capacity;
	}
	
	template <typename T>
	sg_vector<T>::sg_vector(size_t n) {
		initialize_fill_n_aux_(T(), n);
	}
	
	template <typename T>
	sg_vector<T>::sg_vector(size_t n, const T& value) {
		initialize_fill_n_aux_(value, n);
	}
	
	template <typename T>
	sg_vector<T>::sg_vector(const sg_vector& other) {
		start = sg_allocate<T>(other.size());
		finish = sg_uninitialized_copy(other.begin(), other.end(), start);
		end_of_storage = finish;
	}
	
	template <typename T>
	sg_vector<T>::sg_vector(sg_vector&& other) {
		start = other.start;
		finish = other.finish;
		end_of_storage = other.end_of_storage;
		other.start = other.finish = other.end_of_storage = nullptr;
	}
	
	template <typename T>
	typename sg_vector<T>::iterator sg_vector<T>::insert(typename sg_vector<T>::iterator pos, T&& data) {
		if (start == 0) {
			initialize_fill_n_aux_(data);
			return finish;
		}
		if (finish != end_of_storage) {
			if (pos == finish) {
				sg_construct(finish, data);
			} else {
				sg_construct(finish, *(finish - 1));
				sg_move_backward(pos, finish - 1, finish);
				++finish;
				*pos = data;
			}
		} else {
			insert_aux_(pos, data);
		}
		return ++pos;
	}
	
	template <typename T>
	typename sg_vector<T>::iterator sg_vector<T>::insert(typename sg_vector<T>::iterator pos, const_reference data) {
		if (start == 0) {
			initialize_fill_n_aux_(data);
			return finish;
		}
		if (finish != end_of_storage) {
			if (pos == finish) {
				sg_construct(finish, data);
			} else {
				sg_construct(finish, *(finish - 1));
				sg_move_backward(pos, finish - 1, finish);
				++finish;
				*pos = data;
			}
		} else {
			insert_aux_(pos, data);
		}
		return ++pos;
	}
	
	template <typename T>
	typename sg_vector<T>::iterator sg_vector<T>::erase(typename sg_vector<T>::iterator pos) {
		if (pos + 1 != finish) {
			sg_copy(pos + 1, finish, pos);
		}
		--finish;
		sg_destroy(finish);
		return pos;
	}
	
	template <typename T>
	typename sg_vector<T>::iterator sg_vector<T>::erase(typename sg_vector<T>::iterator begin, typename sg_vector<T>::iterator end) {
		iterator pos = sg_copy(end, finish, begin);
		sg_destroy(end, finish);
		finish -= end - begin;
		return begin;
	}
	
	template <typename T>
	void sg_vector<T>::pop_back() {
		--finish;
		sg_destroy(finish);
	}
	
	template <typename T>
	void sg_vector<T>::clear() {
		sg_destroy(start, finish);
		finish = start;
	}

	template <typename T>
	sg_vector<T>::~sg_vector() {
		clear();
		sg_deallocate(start, capacity());
	}
	
}

#endif //SG_STL_SG_VECTOR_H
