#include "catch2/catch.hpp"

#include <iostream>

#include "../include/__memory/sg_reference_count.h"

TEST_CASE( "Test Reference Counters", "[reference counter]" ) {
    using namespace sg;


    SECTION("shared_counter", "rc") {
        class SCImpl: public sg_shared_count {
            void on_zero_shared() noexcept override {
                std::cout << "x";
            }
        };

        SCImpl ssc;
        REQUIRE(ssc.use_count() == 1);
        ssc.add_shared();
        REQUIRE(ssc.use_count() == 2 );
        REQUIRE(ssc.release_shared() == false);
        REQUIRE(ssc.use_count() == 1);
        REQUIRE(ssc.release_shared() == true);
        REQUIRE(ssc.use_count() == 0);
    }

    SECTION("shared_weak_counter") {
        class SCWImpl: public sg_shared_weak_count {
            void on_zero_shared() noexcept override {
                std::cout << "on_zero_shared" << std::endl;
            }

            void on_zero_shared_weak() noexcept override {
                std::cout << "on_zero_shared_weak" << std::endl;
            }
        };
        SCWImpl swc;
        SECTION("shared") {
            REQUIRE(swc.use_count() == 1);
            swc.add_shared();
            REQUIRE(swc.use_count() == 2 );
            swc.release_shared();
            REQUIRE(swc.use_count() == 1);
            swc.release_shared();
            REQUIRE(swc.use_count() == 0);
        }

        SECTION("weak") {
            REQUIRE(swc.use_count() == 1);
            swc.add_weak();
            swc.release_weak();
            swc.release_shared();
        }


        SECTION("weak+") {
            REQUIRE(swc.use_count() == 1);
            swc.add_weak();
            swc.release_shared();
            swc.release_weak();
        }
    }

}