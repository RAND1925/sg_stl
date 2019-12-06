//
// Created by dell on 2019/11/8.
//

#ifndef S06_FAMILY_SG_TREE_H
#define S06_FAMILY_SG_TREE_H

#include "sg_bilist.h"
#include "sg_basic.h"

namespace sg{
	
	template <typename T>
	class sg_tree {
	public:
		using node = sg_bilist_node<T>;
		using link_type = node*;
		
		node* find_node(const T& data) {
			return find_node_from(root_, data));
		}
		
		node* find_node_from(const node *const curr, const T& data) {
			if (curr == nullptr) {
				return nullptr;
			}
			if (data == curr->data) {
				return curr;
			}
			auto result_left = find_node_from(curr->left, data);
			if (result_left == nullptr) {
				return find_node_from(curr->right, data);
			}
			return result_left;
		}
		
		void insert_left(node *const curr,const T& data) {
			curr->left = create_node(std::forward(data));
		}
		
		void insert_right(node *const curr, const T& data) {
			curr->right = create_node(std::forward(data));
		}
		
		void erase_subtree(node *curr) {
			if (curr == nullptr) {
				return;
			}
			erase_subtree(curr->left);
			erase_subtree(curr->right);
			destroy_node(curr);
		}
		sg_tree(T&& data): root_(generate_node(data)){
		
		}
	
	protected:
		
		node* create_node(const T& x) {
			auto new_node = sg::sg_allocator<node>::allocate();
			construct(&(new_node->data), x);
			return new_node;
		};
		void destroy_node(node* node_to_delete){
			destroy(node_to_delete);
		};
	
	private:
		link_type root_;
		
	};
	
}


#endif //S06_FAMILY_SG_TREE_H
