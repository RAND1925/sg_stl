#ifndef SG_STL_SG_SHARED_PTR_H
#define SG_STL_SG_SHARED_PTR_H

#include <memory>
#include <type_traits>

#include "sg_compressed_pair.h"
#include "sg_unique_ptr.h"
#include "sg_allocator.h"
#include "sg_allocator_traits.h"
#include "sg_default_delete.h"
#include "sg_reference_count.h"

namespace sg {

    template <typename T, typename U>
    struct sg_is_compatible_with : public std::is_convertible<std::remove_extent_t<T>*, std::remove_extent_t<U>*> {
    };

    template <typename T, typename U>
    inline constexpr bool sg_is_compatible_with_v = sg_is_compatible_with<T, U>::value;

    template <typename T>
    class sg_enable_shared_from_this;

    template <typename T, typename Delete, typename Allocator>
    class sg_shared_ptr_pointer : public sg_shared_weak_count {
    private:
        /* !
         * 管理的指针，这个指针的删除器，这个管理块的分配器
         */
        sg_compressed_pair<sg_compressed_pair<T, Delete>, Allocator> data;


    public:
        inline sg_shared_ptr_pointer(T p, Delete d, Allocator a)
                : data(sg_compressed_pair<T, Delete>(p, std::move(d)), std::move(a)) {
        }

        [[nodiscard]] const void* get_deleter(const std::type_info& t) const {
            return t == typeid(Delete) ? std::addressof(data.first().second()) : nullptr;
        }

    private:
        /* !
         * 引用计数归零时，使用deleter删除所引用指针，然后deleter失去作用，析构自己。此时只有allocator有用了
         */
        void on_zero_shared() noexcept override {
            data.first().second()(data.first().first());
            data.first().second().~Delete();
        }

        /* !
         * 先根据找回自己的类型，然后构造一个新的构造器（RAII），悉构掉原来的构造器，最后释放自己所在内存块。，
         */
        void on_zero_shared_weak() noexcept override {
            using Al = typename sg_allocator_traits<Allocator>::template rebind_alloc<sg_shared_ptr_pointer>;
            using ATraits = sg_allocator_traits<Al>;
            using PTraits = typename std::pointer_traits<typename ATraits::pointer>;

            Al a(data.second());
            data.second().~Allocator();
            a.deallocate(PTraits::pointer_to(*this), 1);
        }
    };

    template <typename T, typename Allocator>
    struct sg_shared_ptr_emplace : public sg_shared_weak_count {
        template <typename... Args>
        explicit sg_shared_ptr_emplace(Allocator a, Args&& ... args) {
        }
    };

    template <typename T>
    class sg_weak_ptr;

    template <typename T>
    class sg_shared_ptr {
    public:
        using element_type = std::remove_extent_t<T>;
        using pointer = element_type*;
        using reference = typename std::add_lvalue_reference<element_type>::type;
        using weak_type = sg_weak_ptr<T>;

    private:
        pointer ptr;
        sg_shared_weak_count* cntrl;

    public:

        // constructor
        inline constexpr sg_shared_ptr() noexcept: ptr(nullptr), cntrl(nullptr) {}

        inline constexpr sg_shared_ptr(std::nullptr_t) noexcept: ptr(nullptr), cntrl(nullptr) {}

        template <typename Y, typename = std::enable_if_t<sg_is_compatible_with_v<Y, T> > >
        explicit sg_shared_ptr(Y* p) : ptr(p) {
            sg_unique_ptr hold(p);
            // Allocate: Y Delete: T
            using AllocT = shared_ptr_default_allocator_t<Y>;
            using DeleteT = shared_ptr_default_delete<T, Y>;
            using CntrlBlk = sg_shared_ptr_pointer<Y*, DeleteT, AllocT>;
            cntrl = new CntrlBlk(p, DeleteT(), AllocT());
            hold.release();
        }

        // copy constructors
        inline sg_shared_ptr(const sg_shared_ptr& r) noexcept
                : ptr(r.ptr), cntrl(r.cntrl) {
            if (cntrl) {
                cntrl->add_shared();
            }
        }

        template <typename Y, typename = std::enable_if_t<sg_is_compatible_with_v<Y, T> > >
        inline sg_shared_ptr(const sg_shared_ptr<Y>& r) noexcept
                : ptr(r.ptr), cntrl(r.cntrl) {
            if (cntrl) {
                cntrl->add_shared();
            }
        }

        inline sg_shared_ptr(const sg_shared_ptr& r, element_type* p) noexcept
                : ptr(p), cntrl(r.cntrl) {
            if (cntrl) {
                cntrl->add_shared();
            }
        }

        template <typename Y, typename = std::enable_if_t<sg_is_compatible_with_v<Y, T> > >
        inline sg_shared_ptr(const sg_shared_ptr<Y>& r, element_type* p) noexcept
                : ptr(p), cntrl(r.cntrl) {
            if (cntrl) {
                cntrl->add_shared();
            }
        }


        // move constructors
        inline sg_shared_ptr(sg_shared_ptr&& r) noexcept
                : ptr(r.ptr), cntrl(r.cntrl) {
            r.ptr = nullptr;
            r.cntrl = nullptr;
        }

        template <typename Y, typename = std::enable_if_t<sg_is_compatible_with_v<Y, T> > >
        inline sg_shared_ptr(sg_shared_ptr<Y>&& r) noexcept
                : ptr(r.ptr), cntrl(r.cntrl) {
            r.ptr = nullptr;
            r.cntrl = nullptr;
        }


        template <typename Y, typename D, typename = std::enable_if_t<sg_is_compatible_with_v<Y, T> > >
        inline sg_shared_ptr(sg_unique_ptr<Y, D>&& r) noexcept
                : ptr(r.get()) {
            if (ptr == nullptr) {
                cntrl = nullptr;
            }
            {
                using AllocT = shared_ptr_default_allocator_t<Y>;
                using UniPtr = typename sg_unique_ptr<Y, D>::pointer;
                if constexpr (std::is_lvalue_reference_v<D>) {
                    using DeleteT = std::reference_wrapper<std::remove_reference_t<D>>;
                    using CntrlBlk = sg_shared_ptr_pointer<UniPtr, DeleteT, AllocT>;
                    cntrl = new CntrlBlk(r.get(), std::ref(r.get_deleter()), AllocT());
                } else {
                    using DeleteT = D;
                    using CntrlBlk = sg_shared_ptr_pointer<UniPtr, DeleteT, AllocT>;
                    cntrl = new CntrlBlk(r.get(), r.get_deleter(), AllocT());
                }
                enable_weak_this(r.get(), r.get());
            }
            r.release();
        }

        // destructors
        ~sg_shared_ptr() {
            if (cntrl) {
                cntrl->release_shared();
            }
        }

        // copy operators
        sg_shared_ptr& operator=(const sg_shared_ptr& r) noexcept {
            sg_shared_ptr(r).swap(*this);
            return *this;
        }

        template <typename Y, typename = std::enable_if_t<sg_is_compatible_with_v<T, Y> > >
        sg_shared_ptr& operator=(const sg_shared_ptr<Y>& r) noexcept {
            sg_shared_ptr(r).swap(*this);
            return *this;
        }

        // move operators
        sg_shared_ptr& operator=(sg_shared_ptr&& r) noexcept {
            sg_shared_ptr(std::move(r)).swap(*this);
            return *this;
        }

        template <typename Y, typename = std::enable_if_t<sg_is_compatible_with_v<T, Y> > >
        sg_shared_ptr& operator=(sg_shared_ptr<Y>&& r) noexcept {
            sg_shared_ptr(std::move(r)).swap(*this);
            return *this;
        }

        template <typename Y, typename D, typename = std::enable_if_t<sg_is_compatible_with_v<T, Y> > >
        sg_shared_ptr& operator=(sg_unique_ptr<Y, D>&& r) noexcept {
            sg_shared_ptr(std::move(r)).swap(*this);
            return *this;
        }

        // modifiers

        inline void swap(sg_shared_ptr& r) noexcept {
            std::swap(ptr, r.ptr);
            std::swap(cntrl, r.cntrl);
        }

        inline void reset() noexcept {
            sg_shared_ptr().swap(*this);
        }

        template <typename Y, typename = std::enable_if_t<sg_is_compatible_with_v<T, Y> > >
        inline void reset(Y* p) noexcept {
            sg_shared_ptr(p).swap(*this);
        }

        pointer get() const noexcept {
            return ptr;
        }

        typename std::add_lvalue_reference<element_type>::type operator*() const noexcept {
            return *ptr;
        }

        typename std::add_lvalue_reference<element_type>::type operator[](std::ptrdiff_t i) const {
            static_assert(std::is_array_v<T>, "T should be an array type");
            return ptr[i];
        }


        pointer operator->() const noexcept {
            static_assert(!std::is_array_v<T>, "T should be not an array type");
            return *ptr;
        }

        // observers
        [[nodiscard]] long use_count() const noexcept {
            return cntrl ? cntrl->use_count() : 0;
        }

        inline explicit operator bool() const noexcept {
            return get();
        }

    private:
        template <typename Y, typename Orig, std::enable_if_t<std::is_convertible_v<Orig*, const sg_enable_shared_from_this<Y>*> >>
        inline void enable_weak_this(const sg_enable_shared_from_this<Y>* e, Orig* ptr) noexcept {
            using RawY = std::remove_cv<Y>;
            //TODO: implementation
        }

        template <typename Y, bool = std::is_function_v<Y> >
        struct shared_ptr_default_allocator {
            using type = sg_allocator<Y>;
        };

        struct shared_ptr_default_allocator_dummy {
        };
        template <typename Y>

        struct shared_ptr_default_allocator<Y, true> {
            using type = sg_allocator<shared_ptr_default_allocator_dummy>;
        };

        template <typename Y>
        using shared_ptr_default_allocator_t = typename shared_ptr_default_allocator<Y>::type;

        template <typename, typename Y>
        struct shared_ptr_default_delete : public sg_default_delete<Y> {
        };

        template <typename Y, typename U, size_t sz>
        struct shared_ptr_default_delete<Y[sz], U> : public sg_default_delete<Y[sz]> {
        };

        template <typename Y, typename U>
        struct shared_ptr_default_delete<Y[], U> : public sg_default_delete<Y[]> {
        };

        template <typename U>
        friend
        class sg_shared_ptr;

        template <typename U>
        friend
        class sg_weak_ptr;

    };

    template <typename T>
    class sg_enable_shared_from_this {
    private:
        mutable sg_weak_ptr<T> weak_this;


    protected:
        inline constexpr sg_enable_shared_from_this() noexcept {}

        inline sg_enable_shared_from_this(const sg_enable_shared_from_this&) noexcept {}

        inline sg_enable_shared_from_this& operator=(const sg_enable_shared_from_this&) noexcept {
            return *this;
        }

        inline ~sg_enable_shared_from_this() {}


    public:

        inline sg_shared_ptr<T> shared_from_this() {
            return weak_this;
        }

        inline sg_shared_ptr<const T> shared_from_this() const {
            return weak_this;
        }

        inline sg_weak_ptr<T> weak_from_this() {
            return weak_this;
        }

        inline sg_weak_ptr<const T> weak_from_this() const {
            return weak_this;
        }

        template <typename U>
        friend
        class sg_shared_ptr;
    };

    template <typename T>
    class sg_weak_ptr {

    public:
        using element_type = std::remove_extent_t<T>;
        using pointer = element_type*;
        using reference = typename std::add_lvalue_reference<element_type>::type;
        using weak_type = sg_weak_ptr<T>;

    private:
        pointer ptr;
        sg_shared_weak_count* cntrl;

    public:
        inline constexpr sg_weak_ptr() noexcept: ptr(nullptr), cntrl(nullptr) {}

        inline constexpr sg_weak_ptr(std::nullptr_t) noexcept: ptr(nullptr), cntrl(nullptr) {}



        // copy constructors
        inline sg_weak_ptr(const sg_weak_ptr& r) noexcept
                : ptr(r.ptr), cntrl(r.cntrl) {
            if (cntrl) {
                cntrl->add_weak();
            }
        }

        template <typename Y, typename = std::enable_if_t<sg_is_compatible_with_v<Y, T> > >
        inline sg_weak_ptr(const sg_weak_ptr<Y>& r) noexcept
                : ptr(r.ptr), cntrl(r.cntrl) {
            if (cntrl) {
                cntrl->add_weak();
            }
        }

        // move constructors
        inline sg_weak_ptr(sg_weak_ptr&& r) noexcept
        : ptr(r.ptr), cntrl(r.cntrl) {
            r.ptr = nullptr;
            r.cntrl = nullptr;
        }

        template <typename Y, typename = std::enable_if_t<sg_is_compatible_with_v<Y, T> > >
        inline sg_weak_ptr(sg_weak_ptr<Y>&& r) noexcept
        : ptr(r.ptr), cntrl(r.cntrl) {
            r.ptr = nullptr;
            r.cntrl = nullptr;
        }

        template <typename Y, typename = std::enable_if_t<sg_is_compatible_with_v<Y, T> > >
        inline sg_weak_ptr(const sg_shared_ptr<Y>& r) noexcept
                : ptr(r.ptr), cntrl(r.cntrl) {
            if (cntrl) {
                cntrl->add_weak();
            }
        }

        // copy operators
        sg_weak_ptr& operator=(const sg_weak_ptr& r) noexcept {
            sg_weak_ptr(r).swap(*this);
            return *this;
        }

        template <typename Y, typename = std::enable_if_t<sg_is_compatible_with_v<T, Y> > >
        sg_weak_ptr& operator=(const sg_weak_ptr<Y>& r) noexcept {
            sg_weak_ptr(r).swap(*this);
            return *this;
        }

        // move operators
        sg_weak_ptr& operator=(sg_weak_ptr&& r) noexcept {
            sg_weak_ptr(std::move(r)).swap(*this);
            return *this;
        }

        template <typename Y, typename = std::enable_if_t<sg_is_compatible_with_v<T, Y> > >
        sg_weak_ptr& operator=(sg_weak_ptr<Y>&& r) noexcept {
            sg_weak_ptr(std::move(r)).swap(*this);
            return *this;
        }

        template <typename Y, typename = std::enable_if_t<sg_is_compatible_with_v<T, Y> > >
        sg_weak_ptr& operator=(const sg_shared_ptr<Y>& r) noexcept {
            sg_weak_ptr(r).swap(*this);
            return *this;
        }

        // dtor
        ~sg_weak_ptr() {
            if (cntrl) {
                cntrl->release_weak();
            }
        }

        // observer
        inline long use_count() const noexcept {
            return cntrl ? cntrl->use_count() : 0;
        }

        inline pointer get() const noexcept {
            return ptr;
        }

        inline bool expired() const noexcept {
            return !cntrl || cntrl->use_count() == 0;
        }

        sg_shared_ptr<T> lock() const noexcept {
            sg_shared_ptr<T> r;
            r.cntrl = cntrl ? cntrl->lock() : cntrl;
            if (r.cntrl) {
                r.ptr = ptr;
            }
            return r;
        }

        inline void swap(sg_weak_ptr& r) noexcept {
            std::swap(ptr, r.ptr);
            std::swap(cntrl, r.cntrl);
        }

        inline void reset() noexcept {
            sg_weak_ptr().swap(*this);
        }

        typename std::add_lvalue_reference<element_type>::type operator*() const noexcept {
            return *ptr;
        }

        typename std::add_lvalue_reference<element_type>::type operator[](std::ptrdiff_t i) const {
            static_assert(std::is_array_v<T>, "T should be an array type");
            return ptr[i];
        }

        pointer operator->() const noexcept {
            static_assert(!std::is_array_v<T>, "T should be not an array type");
            return *ptr;
        }
    };

    template <typename T>
    struct sg_hash {
        using argument_type = sg_shared_ptr<T>;
        using result_type = size_t;

        inline result_type operator()(const argument_type& shared_p) const noexcept {
            return std::hash<typename argument_type::element_type*>()(shared_p.get());
        }
    };
}

#endif //SG_STL_SG_SHARED_PTR_H
