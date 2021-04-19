#include <iostream>

#include "boost/test/minimal.hpp"

#include "../sg_compressed_pair.h"
#include "../sg_smart_pointer.h"
class Empty {
public:
    int func(){
       return 1;
    };
};
class StillEmpty{};

int test_main1(int argc, char *argv[])           // 测试主函数，不需要在定义main()
{
    using namespace sg;

    sg_compressed_pair<Empty, int> pr;

    std::cout << sizeof(pr);
    BOOST_CHECK(pr.first().func() == 1);

    sg_compressed_pair<int, int> pr2(1, 2);
    BOOST_CHECK(pr2.first() == 1);
    BOOST_CHECK(pr2.second() == 2);
    pr2.first() = 3;
    BOOST_CHECK(pr2.first() == 3);
    return 0;
}

template <typename T>
class a_deleter {
    void operator()(T* ptr) {
        std::destroy_at(ptr);
        std::cout << "end";
    }
};

int test_main(int argc, char *argv[])           // 测试主函数，不需要在定义main()
{
    using namespace sg;
    auto made = sg_make_unique<int>(1);
    const auto d = made.get_deleter();
    BOOST_CHECK(*made.get() == 1);
    auto duplicated = std::move(made);
    BOOST_CHECK(*duplicated.get() == 1);
    auto p = duplicated.release();
    BOOST_CHECK(1);

    auto custom = sg_unique_ptr<int>(new int(1));

    return 0;
}