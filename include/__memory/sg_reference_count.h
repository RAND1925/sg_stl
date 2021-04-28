#ifndef SG_STL_SG_REFERENCE_COUNT_H
#define SG_STL_SG_REFERENCE_COUNT_H

namespace sg {
    class sg_shared_count {
    protected:
        /* !
         * 引用计数
         */
        long shared_owners;


    public:
        inline explicit sg_shared_count(long refs = 1) noexcept : shared_owners(refs) {
        }

        sg_shared_count(const sg_shared_count&) = delete;

        sg_shared_count& operator=(const sg_shared_count&) = delete;

        void add_shared() noexcept {
            ++shared_owners;
        }
        /* !
         * 减少引用计数，若归零，则调用虚函数on_zero_shared
         * @return : 是否归零
         */
        bool release_shared() noexcept {
            --shared_owners;
            if (shared_owners == 0) {
                on_zero_shared();
                return true;
            } else {
                return false;
            }
        }

        [[nodiscard]] inline long use_count() const noexcept {
            return shared_owners;
        }

    protected:
        /* !
         *  引用计数归零时的行为
         */
        virtual ~sg_shared_count() {}

    private:
        virtual void on_zero_shared() noexcept = 0;
    };

    class sg_shared_weak_count: private sg_shared_count {
    private:
        /* !
         * weak_ptr的弱引用计数，以及若有的shared_ptr，则加一
         */
        long shared_weak_owners;


    public:
        /* !
         * 强引用计数默认为1，若引用计数默认也为1（代表有强引用）
         */
        inline explicit sg_shared_weak_count(long refs = 1) noexcept
            : sg_shared_count(refs), shared_weak_owners(refs) {
        }

        [[nodiscard]] inline long use_count() const noexcept {
            return sg_shared_count::use_count();
        }

        inline void add_shared() noexcept {
            sg_shared_count::add_shared();
        };

        inline void add_weak() noexcept {
            ++shared_weak_owners;
        }
        /*
         * 减少引用计数，若归零，则弱引用计数减1（代表强引用计数的1）。强引用计数归零后就不用再考虑增加
         */
        inline void release_shared() noexcept {
            if (sg_shared_count::release_shared()) {
                release_weak();
            }
        }
        /*
         * 减少弱引用计数，若归零，调用递归函数
         */
        inline void release_weak() noexcept {
            --shared_weak_owners;
            if (shared_weak_owners == 0) {
                on_zero_shared_weak();
            }
        }

        sg_shared_weak_count* lock(){
            return shared_owners ? this : nullptr;
        };
    protected:
        virtual ~sg_shared_weak_count() {}

    private:
        /* !
         *  若引用计数归零时的行为
         */
        virtual void on_zero_shared_weak() noexcept = 0;
    };

}

#endif //SG_STL_SG_REFERENCE_COUNT_H

