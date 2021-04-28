#ifndef SG_STL_SG_ALLOCATOR_BASE_H
#define SG_STL_SG_ALLOCATOR_BASE_H

#include <new>
namespace sg {
    template<class T>
    static T* sg_addressof(const T &arg) {
        return reinterpret_cast<T *>(
                &const_cast<char &>(
                        reinterpret_cast<const volatile char &>(arg)));
    }

    template<typename T, typename ...Args>
    inline constexpr void sg_construct_at(T *p, Args &&...args) {
        ::new(static_cast<void *>(p)) T(std::forward<Args>(args)...);
    }

    template<typename T>
    inline constexpr void sg_destroy_at(T *p) {
        p->~T();
    }
}

#endif //SG_STL_SG_ALLOCATOR_BASE_H
