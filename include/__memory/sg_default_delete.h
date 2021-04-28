#ifndef SG_STL_SG_DEFAULT_DELETE_H
#define SG_STL_SG_DEFAULT_DELETE_H

#include <type_traits>


namespace sg {
    template <typename T>
    inline void sg_delete_directly(std::remove_extent_t<T>* ptr) noexcept {
        if constexpr (std::is_array_v<T>) {
            delete[] ptr;
        } else {
            delete ptr;
        }
    }


    template <typename T>
    struct sg_default_delete {
        constexpr sg_default_delete() noexcept = default;

        template<typename U>
        constexpr sg_default_delete(const sg_default_delete<U>&) noexcept {}

        inline void operator()(std::remove_extent_t<T>* ptr) const noexcept {
            sg_delete_directly<T>(ptr);
        }
    };

}
#endif //SG_STL_SG_DEFAULT_DELETE_H
