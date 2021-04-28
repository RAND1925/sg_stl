#ifndef SG_STL_SG_ALLOCATOR_TRAITS_H
#define SG_STL_SG_ALLOCATOR_TRAITS_H

#include <limits>

#include "sg_allocator_base.h"

namespace sg {

    template <typename Allocator, typename U>
    struct sg_allocator_traits_rebind {
    };

    template <
            template <typename, typename...>
            typename Allocator, typename T, typename U, typename... Args>
    struct sg_allocator_traits_rebind<Allocator<T, Args...>, U> {
        using type = Allocator<U, Args...>;
    };

    template <typename Allocator, typename U>
    using sg_allocator_traits_rebind_t = typename sg_allocator_traits_rebind<Allocator, U>::type;

    template<typename Allocator>
    struct sg_allocator_traits {
        using allocator_type = Allocator;
        using value_type = typename Allocator::value_type;
        using pointer = value_type *;
        using const_pointer = typename std::pointer_traits<pointer>::template rebind<const value_type>;
        using void_pointer = typename std::pointer_traits<pointer>::template rebind<void>;
        using const_void_pointer = typename std::pointer_traits<pointer>::template rebind<const void>;
        using difference_type =typename  Allocator::difference_type;
        using size_type = typename Allocator::size_type;
        using is_always_equal = typename std::is_empty<Allocator>::type;

        [[nodiscard]] static inline constexpr pointer allocate(Allocator &a, size_type n) {
            return a.allocate(n);
        }

        static inline constexpr void deallocate(Allocator &a, pointer p, size_type n) {
            a.deallocate(p, n);
        }

        template<typename T, typename ...Args>
        static inline constexpr void construct(Allocator &a, T *p, Args &&...args) {
            sg_construct_at(p, std::forward<Args>(args)...);
        }

        template<typename T>
        static inline constexpr void destroy(Allocator &a, T *p) {
            sg_destroy_at(p);
        }

        template<typename U>
        using rebind_alloc = sg_allocator_traits_rebind_t<Allocator, U>;

        template<typename U>
        using rebind_traits = sg_allocator_traits<rebind_alloc<U> >;

        static inline constexpr size_type max_size(const Allocator& a) noexcept {
            return std::numeric_limits<size_type>::max() / sizeof(value_type);
        }
    };
}
#endif //SG_STL_SG_ALLOCATOR_TRAITS_H
