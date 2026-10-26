/**
 * @file example.cpp
 *
 * @copyright © 2026 s3mat3
 * This code is licensed under the MIT License, see the LICENSE file for
 * details
 *
 * @brief
 *
 * @author s3mat3
 */
#include <iostream>
#include <list>
#include <vector>
#include <unordered_map>

#define MOTO_USE_LOG 1
#define MOTO_USE_ASSERT 1
#define MOTO_USE_TRACE 1
#include "debug.hpp"
#include "logger.hpp"

// Class with to_string()
struct User {
    std::string name;
    int age;
    std::string to_string() const {
        return "User(name: " + name + ", age: " + std::to_string(age) + ")";
    }
};

// Classes without to_string()
struct Point {
    int x, y;
};


using namespace moto::debug;

void func1() {std::cout << "Func1 : " <<  get_backtrace() << std::endl;}

void func2() {func1(); std::cout << "Func2 : " << get_backtrace() << std::endl;}

int main() {
    std::string input = "Hello\nWorld\t\x01\x7f\x82\xff";
    std::cout << moto::debug::toReadableCtrlCode(input) << std::endl;
    // 1. string system
    std::string s1 = "Hello World";
    std::string_view s2 = "String View";
    std::cout << "String: " << dump_var_simple(s1) << "\n";
    std::cout << "SView:  " << dump_var_simple(s2) << "\n";

    // 2. C format string
    const char* c_str = "Line1\nLine2\tTabbed Hello\nWorld\t\x01\x7f\x82\xff";
    std::cout << "C-Str:  " << dump_var_simple(c_str) << "\n";

    // 3. numerical system
    int i = 42;
    double d = 3.14159;
    std::cout << "Int:    " << dump_var_simple(i) << "\n";
    std::cout << "Double: " << dump_var_simple(d) << "\n";

    // 4. pointer system
    int val = 100;
    int* ptr = &val;
    int* null_ptr = nullptr;
    std::cout << "Ptr:    " << dump_var_simple(ptr) << "\n";
    std::cout << "Null:   " << dump_var_simple(null_ptr) << "\n";

    // 5. class system
    User user{"Alice", 30};
    Point pt{10, 20};
    std::cout << "User:   " << dump_var_simple(user) << "\n";
    std::cout << "Point:  " << dump_var_simple(pt) << "\n";

    // Vector
    std::vector<int> vec = {1, 2, 3};
    std::cout << "Vector: " << dump_var_simple(vec) << "\n";

    // List of strings
    std::list<std::string> lst = {"Apple", "Banana", "Cherry"};
    std::cout << "List:   " << dump_var_simple(lst) << "\n";

    // Unordered Map
    std::unordered_map<std::string, int> map = {{"Alice", 20}, {"Bob", 25}};
    std::cout << "Map:    " << dump_var_simple(map) << "\n";
    LOG_I(MOTO_VAR_STRING(map));

    // Nested structure: vector of maps
    std::vector<std::unordered_map<std::string, int>> nested = {
        {{"A", 1}}, {{"B", 2}}
    };
    std::cout << "Nested: " << moto::debug::dump_var_simple(nested) << "\n";
    MOTO_VAR_DUMP(nested);
    //assertion
    auto xyz = 1;
    if (MOTO_ASSERT(xyz < 0, "check-xyz")) {
        std::cout << "Hello" << std::endl;
    } else {std::cout << "Abort" << std::endl;}
    // back trace
    func2();
    std::cout << MOTO_TRACE() << std::endl;
    return 0;
}



