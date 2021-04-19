#ifndef SG_STL_SG_SMART_POINTER_H
#define SG_STL_SG_SMART_POINTER_H

#include <memory>

#include "sg_compressed_pair.h"

// do not support array
namespace sg {
    template <typename T>
    class sg_smart_ptr {
    };
    template <typename T>
    struct sg_default_deleter {
        void operator()(T* ptr) {
            std::destroy_at(ptr);
        }
    };

    template <typename T, typename Deleter = sg_default_deleter<T> >
    class sg_unique_ptr: public sg_smart_ptr<T> {
    public:
        using element_type = T;
        using deleter_type = Deleter;
        using pointer = T*;

        // constructor
        constexpr sg_unique_ptr() noexcept {}
        constexpr sg_unique_ptr(std::nullptr_t null_ptr) noexcept {}

        explicit sg_unique_ptr(pointer p) noexcept : ptr(p, sg_default_deleter<T>()) {}
        inline sg_unique_ptr(sg_unique_ptr&& u) noexcept : ptr(u.release(), sg_default_deleter<T>(u.get_deleter())) {}

        template< class U, class E >
        sg_unique_ptr(sg_unique_ptr<U, E>&& u) noexcept
        : ptr(u.release(), std::forward<deleter_type>(u.get_deleter())){
        }


        // destructor
        ~sg_unique_ptr() {
            reset();
        }

        // assignment
        sg_unique_ptr& operator=(std::nullptr_t) noexcept {
            reset();
            return *this;
        }

        // deleted copying func
        sg_unique_ptr(sg_unique_ptr const&) = delete;
        sg_unique_ptr& operator=(sg_unique_ptr&) = delete;

        // getter
        inline typename std::add_lvalue_reference_t<T> operator*() const {
            return *get();
        };

        inline pointer operator->() const noexcept {
            return get();
        }

        inline pointer get() const noexcept {
            return ptr.first();
        }

        inline Deleter& get_deleter() noexcept {
            return ptr.second();
        };

        inline const Deleter& get_deleter() const noexcept {
            return ptr.second();
        };

        inline explicit operator bool() const {
            return ptr.first();
        }

        // modifier
        inline pointer release() noexcept {
            auto old = ptr.first();
            ptr.first() = pointer();
            return old;
        }

        inline void reset(pointer p = pointer()) noexcept {
            auto old = ptr.first();
            ptr.first() = p;
            if (old) {
                ptr.second()(old);
            }
        }

        inline void swap(sg_unique_ptr& u) noexcept {
            ptr.swap(u.ptr);
        }

    private:
        sg_compressed_pair<T*, Deleter> ptr;
    };

    template <typename T1, typename Deleter1, typename T2, typename Deleter2>
    inline bool operator== (const sg_unique_ptr<T1, Deleter1>& x, const sg_unique_ptr<T2, Deleter2>& y) {
        return x.get() == y.get();
    }

    template <typename T1, typename Deleter1, typename T2, typename Deleter2>
    inline bool operator!= (const sg_unique_ptr<T1, Deleter1>& x, const sg_unique_ptr<T2, Deleter2>& y) {
        return !(x == y);
    }

    template <typename T, typename Deleter>
    void swap(sg_unique_ptr<T, Deleter>& x, sg_unique_ptr<T, Deleter>& y) {
        x.swap(y);
    }

    template <typename T>
    struct unique_if {
        using unique_single = sg_unique_ptr<T>;
    };

    template <typename T>
    struct unique_if<T[]> {
        using unique_array_unknown_bound = sg_unique_ptr<T[]>;
    };

    template <typename T, size_t n>
    struct unique_if<T[n]> {
        using unique_array_known_bound = sg_unique_ptr<T[n]>;
    };

    template<typename T, typename... Args>
    typename unique_if<T>::unique_single sg_make_unique(Args... args) {
        return sg_unique_ptr<T>(new T(std::forward<Args>(args)...));
    }

}
#endif //SG_STL_SG_SMART_POINTER_H
