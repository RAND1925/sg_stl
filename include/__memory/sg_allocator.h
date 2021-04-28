#ifndef SG_STL_SG_ALLOCATOR_H
#define SG_STL_SG_ALLOCATOR_H

#include <new>
#include "../sg_basic.h"
namespace sg {

    template <typename T>
    struct sg_allocator {
        using value_type = T;
        using size_type = std::size_t;
        using difference_type = std::ptrdiff_t;
        using propagate_on_container_remove_assignment = std::true_type;

        constexpr sg_allocator() noexcept {}

        constexpr sg_allocator(const sg_allocator& other) noexcept {}

        template<typename U>
        constexpr sg_allocator(const sg_allocator<U>& other) noexcept {}

        ~sg_allocator() {}

        [[nodiscard]] inline constexpr T* allocate(std::size_t n) {
            return ::operator new(n * sizeof(T));
        }

        inline constexpr void deallocate(T* p, std::size_t n) noexcept {
            ::operator delete(p);
        }
    };

	template <typename T>
	static T* sg_allocate(const size_t& n = 1) {
		return static_cast<T*>(::operator new((sizeof(T) * n)));
	}

	template <typename T>
	static void sg_deallocate(T* p, size_type n = 1) {
		if (n != 0) {
			::operator delete(p);
		}
	}


}

#endif //SG_STL_SG_ALLOCATOR_H
