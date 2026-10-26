/**
 * @file debug_test.cpp
 *
 * @copyright © 2026 s3mat3
 * This code is licensed under the MIT License, see the LICENSE file for details
 *
 * @brief unit test for debug.hpp
 *
 * @author s3mat3
 */

#define DOCTEST_CONFIG_IMPLEMENT_WITH_MAIN
#include "doctest.h"
#include <list>
#include <unordered_map>
#include <vector>
#include <string>

#include "debug.hpp"

using namespace moto::debug;

TEST_CASE("Testing dump_var_simple with void* and others") {

    SUBCASE("Generic pointers (typed)") {
        int x = 10;
        int* px = &x;
        std::string res = dump_var_simple(px);

        // If there is a type, it should be "value @address"
        CHECK(res.find("10 @") == 0);
    }

    SUBCASE("Void pointers") {
        int x = 10;
        void* pv = &x; // cast int* to void*

        std::string res = dump_var_simple(pv);

        // In the case of void*, it should be "void* @address" without
        // dereference.
        CHECK(res.find("void* @") == 0);
        // The content "10" must not appear
        // (because the content is unknown in void*)
        CHECK(res.find("10") == std::string::npos);
    }

    SUBCASE("Void null pointer") {
        void* pv_null = nullptr;
        // Even if it is void*, nullptr should have a common format
        CHECK(dump_var_simple(pv_null) == "nullptr @0x0");
    }

    SUBCASE("Recursive pointer (pointer to pointer)") {
        int x = 100;
        int* px = &x;
        int** ppx = &px;

        std::string res = dump_var_simple(ppx);

        // Processed recursively: formatted as "100 @address1 @address2"
        // (Depending on the implementation, in the current code
        // dump_var_simple(*ppx) returns "100 @addr1" and
        // appends " @addr2" to it.)
        CHECK(res.find("100 @") == 0);

        // Check that the address symbol '@' appears twice
        int count = 0;
        for(char c : res) if(c == '@') count++;
        CHECK(count == 2);
    }
}

TEST_CASE("Testing extended dump_var_simple with containers") {

    SUBCASE("Sequential Containers (Vector/List)") {
        // Test vector
        std::vector<int> v = {1, 2, 3};
        CHECK(dump_var_simple(v) == "seq(1, 2, 3)");

        // Test list
        std::list<std::string> l = {"a", "b"};
        CHECK(dump_var_simple(l) == "seq(a, b)");

        // Empty continer
        std::vector<int> empty_v;
        CHECK(dump_var_simple(empty_v) == "seq()");
    }

    SUBCASE("Map Containers (unordered_map)") {
        // Test map (order is unspecified, so check if it is included)
        std::unordered_map<std::string, int> m = {{"Key1", 10}};
        CHECK(dump_var_simple(m) == "map([Key1 => 10])");

        // Multi-element map
        std::unordered_map<int, std::string> m2 = {{1, "One"}, {2, "Two"}};
        std::string res = dump_var_simple(m2);
        CHECK(res.find("map(") == 0);
        CHECK(res.find("[1 => One]") != std::string::npos);
        CHECK(res.find("[2 => Two]") != std::string::npos);

        // Empty map
        std::unordered_map<int, int> empty_m;
        CHECK(dump_var_simple(empty_m) == "map()");
    }

    SUBCASE("Nested Containers (The real power of recursion)") {
        // Vector of Vectors
        std::vector<std::vector<int>> nested_v = {{1, 2}, {3, 4}};
        CHECK(dump_var_simple(nested_v) == "seq(seq(1, 2), seq(3, 4))");

        // Vector of Maps
        std::vector<std::unordered_map<std::string, int>> v_of_m = {
            {{"A", 1}},
            {{"B", 2}}
        };
        std::string res = dump_var_simple(v_of_m);
        CHECK(res.find("seq(") == 0);
        CHECK(res.find("map([A => 1])") != std::string::npos);
        CHECK(res.find("map([B => 2])") != std::string::npos);

        // Map of Vectors
        std::unordered_map<std::string, std::vector<int>> m_of_v = {
            {"nums", {10, 20}}
        };
        CHECK(dump_var_simple(m_of_v) == "map([nums => seq(10, 20)])");
    }

    SUBCASE("Integration with basic types") {
        // When pointers and classes are mixed in a container
        struct User {
            std::string to_string() const { return "UserObj"; }
        };

        int val = 100;
        int* ptr = &val;

        // Vector < variant-like > : Because they actually need to be of
        // the same type
        // Here we test with a Vector of pointers
        std::vector<int*> ptr_vec = {ptr, nullptr};
        std::string res = dump_var_simple(ptr_vec);

        CHECK(res.find("seq(") == 0);
        CHECK(res.find("100 @") != std::string::npos);
        CHECK(res.find("nullptr @0x0") != std::string::npos);
    }
}
