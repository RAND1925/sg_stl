//
// Created by dell on 2019/11/6.
//

#ifndef SG_STL_SG_BILIST_H
#define SG_STL_SG_BILIST_H


template <typename T>
struct sg_bilist_node{
	using pointer = sg_bilist_node<T>*;
	pointer parent;
	pointer left;
	pointer right;
	T data;
};


#endif //SG_STL_SG_BILIST_H
