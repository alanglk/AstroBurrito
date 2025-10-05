#include "astro/math/math.hpp"
#include "astro_test.hpp"

TEST(example) {
    int a = 0;
    int b = 0;
    ASSERT_EQ(a, b);
    return true;
}

int main(){
    bool all_success = run_all_tests();
    return 0;
}

