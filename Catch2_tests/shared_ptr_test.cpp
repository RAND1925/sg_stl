#include "catch2/catch.hpp"

#include <iostream>

#include "../include/__memory/sg_shared_ptr.h"

TEST_CASE( "Test Shared ptr") {
    using namespace sg;

    SECTION("shared_ptr", "[rc]") {
        sg_shared_ptr<int> sp(new int(4));
        REQUIRE(*sp.get() == 4);
        REQUIRE(sp);
        REQUIRE(sp.use_count() == 1);
        *sp = 2;
        REQUIRE(*sp == 2);
    }
}


TEST_CASE( "Test Shared ptr copy") {
    using namespace sg;

    SECTION("shared_ptr", "[rc]") {
        sg_shared_ptr<int> sp(new int(4));
        REQUIRE(sp.use_count() == 1);
        REQUIRE(*sp == 4);
        sg_shared_ptr<int> sp_dup(sp);
        REQUIRE(*sp == 4);
        REQUIRE(sp.use_count() == 2);
        REQUIRE(sp_dup.use_count() == 2);
    }
}

TEST_CASE( "Test Shared ptr release") {
    using namespace sg;

    SECTION("shared_ptr", "[rc]") {
        sg_shared_ptr<int> sp(new int(4));
        sp = nullptr;
        REQUIRE(!sp);
    }
}

TEST_CASE( "Test Weak ptr") {
    using namespace sg;

    SECTION("weak_ptr", "[rc]") {
        sg_weak_ptr<int> wp;
        sg_shared_ptr<int> sp(new int(4));
        REQUIRE(wp.use_count() == 0);
        {
            sg_shared_ptr<int> spd(sp);
            wp = sp;
            REQUIRE(*wp.get() == 4);
            REQUIRE(wp.use_count() == 2);
        }
        REQUIRE(wp.use_count() == 1);
    }
}
