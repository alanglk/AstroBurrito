#pragma once

#include <iostream>
#include <functional>
#include <vector>

#define ASSERT_EQ(actual, expected) \
    if ((actual) != (expected)){ \
        std::cerr << __FILE__ << ":" << __LINE__ << ": Assertion failed: expected `" << (expected) << "`, got `" << (actual) << "`\n"; \
        return false; \
    }
#define ASSERT_NEQ(actual, expected) \
    if ((actual) == (expected)){ \
        std::cerr << __FILE__ << ":" << __LINE__ << ": Assertion failed: expected distinct but got ecual: `" << (actual) << "`\n"; \
        return false; \
    }
#define ASSERT_TRUE(condition) \
    if (!(condition)){ \
        std::cerr << __FILE__ << ":" << __LINE__ << ": Assertion failed: " #condition << std::endl; \
        return false; \
    }
#define ASSERT_FALSE(condition) ASSERT_TRUE(!(condition))

struct Test;
std::vector<Test>& tests();
struct Test {
    std::string name;
    std::function<bool(void)> func;
    Test(const std::string &name, const std::function<bool(void)> &func)
        : name(name), func(func) {
      tests().push_back(*this);
    };
};
inline std::vector<Test>& tests(){
    static std::vector<Test> instance;
    return instance;
};

#define TEST(name) \
    bool test_func_##name(); \
    static Test test_##name(#name, test_func_##name); \
    bool test_func_##name() \

inline bool run_all_tests(){
    bool all_success = true;
    for(const auto test: tests()){
        if(test.func()){
            std::cout << "PASSED ✅ ";
        }else{
            std::cout << "FAILED ❌ ";
            all_success = false;
        }
        std::cout << "TEST " << test.name << "\n";
    }
    
    if(all_success){
        std::cout << "All tests finished successfully🎉\n";
    }
    
    return all_success;
}