//
// Created by rand1925 on 2021/4/16.
//

#ifndef SG_STL_SG_COMPRESSED_PAIR_H
#define SG_STL_SG_COMPRESSED_PAIR_H

#include <type_traits>

namespace sg {
    template <typename T, size_t idx, bool can_be_empty_base = std::is_empty_v<T>&& !std::is_final_v<T>>
    struct sg_compressed_pair_element {

    public:
        using value_type = T;
        using reference = T&;
        using const_reference = const T&;

        inline constexpr sg_compressed_pair_element() :value() {
        }

        template<typename U>
        inline constexpr explicit sg_compressed_pair_element(U&& u): value(std::forward<U>(u)) {
        }

        inline reference get() noexcept {
            return value;
        }

        inline const_reference get() const noexcept {
            return value;
        }
    private:
        T value;

    };

    template <typename T, size_t idx>
    struct sg_compressed_pair_element<T, idx, true> : private T {
    public:
        using value_type = T;
        using reference = T&;
        using const_reference = const T&;

        inline constexpr sg_compressed_pair_element(): T() {
        }

        template<typename U>
        inline constexpr explicit sg_compressed_pair_element(U&& u): T(std::forward<U>(u)) {
        }

        inline reference get() noexcept {
            return *this;
        }

        inline const_reference get() const noexcept {
            return *this;
        }
    };

    template <typename T1, typename T2, bool same = !std::is_same<T1, T2>::value>
    struct sg_compressed_pair: private sg_compressed_pair_element<T1, 0>, private sg_compressed_pair_element<T2, 1> {
    public:
        using Base1 = sg_compressed_pair_element<T1, 0>;
        using Base2 = sg_compressed_pair_element<T2, 1>;

        sg_compressed_pair(){}

        template<typename U1, typename U2>
        sg_compressed_pair(U1&& t1, U2&& t2) : Base1(std::forward<U1>(t1)), Base2(std::forward<U2>(t2)) {}

        [[nodiscard]] inline typename Base1::const_reference first() const noexcept {
            return Base1::get();
        }

        [[nodiscard]] inline typename Base1::reference first() noexcept {
            return Base1::get();
        }

        [[nodiscard]] typename Base2::const_reference second() const noexcept {
            return static_cast<const Base2&>(*this).get();
        }

        [[nodiscard]] typename Base2::reference second() noexcept {
            return Base2::get();
        }

        inline void swap(sg_compressed_pair& x) noexcept(std::is_nothrow_swappable_v<T1> && std::is_nothrow_swappable_v<T2>) {
            swap(first(), x.first());
            swap(second(), x.second());
        }
    };

    template<typename T1, typename T2>
    inline void swap(sg_compressed_pair<T1, T2>& x, sg_compressed_pair<T1, T2>& y) noexcept(std::is_nothrow_swappable_v<T1> && std::is_nothrow_swappable_v<T2>) {
        x.swap(y);
    }
}

#endif //SG_STL_SG_COMPRESSED_PAI_H
