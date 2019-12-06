//
// Created by dell on 2019/9/18.
//

#ifndef SG_STL_SG_VECTOR_H
#define SG_STL_SG_VECTOR_H

#include "../sg_stl/sg_basic.h"
#include "../sg_stl/sg_algorithm.h"
#include "../sg_stl/sg_allocator.h"
#include <type_traits>

namespace sg {
	
	template<typename T>
	class sg_vector {
	public:
		using value_type = T;
		using reference = T&;
		using pointer = T*;
		using const_reference = const T&;
		using const_pointer = const T*;
		using iterator = pointer;
		using const_iterator = const_pointer;
		using iterator_category = sg_random_access_iterator_tag;
	protected:
		iterator start;
		iterator finish;
		iterator end_of_storage;
		void insert_aux_(iterator pos, const_reference data) {
			const size_t old_size = size();
			const size_t new_size = old_size ? 2 * old_size : 1;
			iterator new_start = nullptr;
			try {
				new_start = sg_allocate<T>(new_size);
			} catch (sg_out_of_memory_exception& e){
				std::cout << e.what();
				throw;
			}
			iterator new_finish = sg_copy(start, pos, new_start);
			sg_construct(new_finish, data);
			++new_finish;
			new_finish = sg_copy(pos, finish, new_finish);
			sg_destroy(start, finish);
			sg_deallocate(start, finish - start);
			start = new_start;
			finish = new_finish;
			end_of_storage = start + new_size;
			
		}
	public:
		sg_vector()= default;
		iterator begin() {
			return start;
		}
		
		iterator end() {
			return finish;
		}
		size_t size() const {
			return finish - start;
		}
		
		size_t capacity() const {
			return end_of_storage - start;
		}
		
		bool empty() const {
			return start == finish;
		}
		
		reference at(size_t n) {
			if (n < size()) {
				return start[n];
			}
			//todo:: wrpmg
		}
		reference operator[] (size_t n) {
			return *(start + n);
		}
		
		reference front() {
			return *start;
		}
		
		reference back() {
			return *(finish - 1);
		}
		void push_back(const_reference data){
			if (finish != end_of_storage) {
				sg_allocator<T>::construct(finish, data);
			}
			else {
				insert_aux_(finish, data);
			}
		}
		
		iterator insert(const_iterator pos, const_reference data){
			if (finish != end_of_storage) {
				if (pos == finish) {
					sg_allocator<T>::construct(finish, data);
				} else {
					sg_allocator<T>::construct(finish, *(finish - 1));
					sg_copy_backward(pos, finish - 1, finish);
					++finish;
					*pos = data;
				}
			} else {
				insert_aux_(pos, data);
			}
			return ++pos;
		}
		
		void pop_back(){
			--finish;
			destroy(finish);
		}
		
		iterator erase(iterator pos) {
			if (pos + 1 != finish) {
				sg_copy(pos + 1, finish, pos);
			}
			--finish;
			destroy(finish);
			return pos;
		}
		iterator erase(iterator begin, iterator end) {
			iterator pos = sg_copy(end, finish, begin);
			sg_destroy(end, finish);
			finish -= end - begin;
			return begin;
		}
		void clear() {
			erase(start, finish);
		}
		~sg_vector(){
			clear();
		}
		
	};
}

#endif //SG_STL_SG_VECTOR_H
