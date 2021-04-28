#ifndef SG_STL_SG_UNINITIALIZED_MEMORY_ALGO_H
#define SG_STL_SG_UNINITIALIZED_MEMORY_ALGO_H

namespace sg {
    template <typename InputIter, typename ForwardIter>
    ForwardIter sg_uninitialized_copy(InputIter first, InputIter last, ForwardIter result) {
        for (; first != last; ++first, ++result) {
            sg_construct(sg_addressof(*result), *first);
        }
        return result;
    }
    template <typename InputIter, typename ForwardIter>
    ForwardIter sg_uninitialized_copy_n(InputIter first, size_t n, ForwardIter result) {
        for (; n > 0; --n, ++first, ++result) {
            sg_construct(sg_addressof(*result), *first);
        }
        return result;
    }

    template <typename InputIter, typename ForwardIter>
    ForwardIter sg_uninitialized_move(InputIter first, InputIter last, ForwardIter result) {
        for (; first != last; ++first, ++result) {
            sg_construct(sg_addressof(*result), std::move(*first));
        }
        return result;
    }


    template <typename InputIter, typename ForwardIter>
    ForwardIter sg_uninitialized_move_n(InputIter first, size_t n, ForwardIter result) {
        for (; n > 0; --n, ++first) {
            sg_construct(sg_addressof(*result), std::move(*first));
        }
        return result;
    }

    template <typename ForwardIter, typename T>
    ForwardIter sg_uninitialized_fill(ForwardIter first, ForwardIter last, const T& value) {
        for (; first != last; ++first) {
            sg_construct(sg_addressof(*first), value);
        }
        return first;
    }

    template <typename ForwardIter, typename T>
    ForwardIter sg_uninitialized_fill_n(ForwardIter first, size_t n, const T& value) {
        for (; n > 0; --n, ++first) {
            sg_construct(sg_addressof(*first), value);
        }
        return first;
    }



    template <typename ForwardIter>
    ForwardIter sg_destroy(ForwardIter first, ForwardIter last) {
        for (; first != last; ++first) {
            sg_destroy(sg_addressof(*first));
        }
        return first;
    }
}
#lastif //SG_STL_SG_UNINITIALIZED_MEMORY_ALGO_H
