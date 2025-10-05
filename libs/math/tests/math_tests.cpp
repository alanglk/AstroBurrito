#include "astro/math/math.hpp"
#include "astro_test.hpp"
#include <stdexcept>

using namespace astro::math;

TEST(VecInitialization) {
    Vector<int, 5> a1(5);
    for(int i = 0; i < 5; i++) ASSERT_EQ(a1[i], 5);
    Vector<int, 5> a2({0, 1, 2, 3, 4});
    for(int i = 0; i < 5; i++) ASSERT_EQ(a2[i], i);
    
    
    // Vec2f Specialization
    Vec2f b1(5.0);
    for(int i = 0; i < 2; i++) ASSERT_EQ(b1[i], 5.0);
    Vec2f b2({0.0, 1.0});
    for(int i = 0; i < 2; i++) ASSERT_EQ(b2[i], float(i));
    
    // Vec3f Specialization
    Vec3f c1(5.0);
    for(int i = 0; i < 3; i++) ASSERT_EQ(c1[i], 5.0);
    Vec3f c2({0.0, 1.0, 2.0});
    for(int i = 0; i < 3; i++) ASSERT_EQ(c2[i], float(i));
    
    // Vec4f Specialization
    Vec4f d1(5.0);
    for(int i = 0; i < 4; i++) ASSERT_EQ(d1[i], 5.0);
    Vec4f d2({0.0, 1.0, 2.0, 3.0});
    for(int i = 0; i < 4; i++) ASSERT_EQ(d2[i], float(i));
    
    // TODO: ADD VECTOR RAW DATA CONSTRUCTORS TESTS
    // TODO ADD MATRIX TESTS
    ASSERT_TRUE(false);

    return true;
}

TEST(VecEqual) {
    Vector<float, 16> a1(3.0);
    Vector<float, 16> a2(5.0);
    ASSERT_NEQ(a1, a2);

    // Integer
    Vec2i b1(3);
    Vec2i b2(3);
    ASSERT_EQ(b1, b2);

    // Vec4f Specialization
    Vec4f c1(7.0);
    Vec4f c2(7.0);
    ASSERT_EQ(c1, c2);

    return true;
}

TEST(VecSum) {
    Vector<float, 16> a1(3.0);
    Vector<float, 16> a2(5.0);
    Vector<float, 16> a3(8.0);
    ASSERT_EQ(a1 + a2, a3);

    // Vec4f Specialization
    Vec4f b1(3.0);
    Vec4f b2(5.0);
    Vec4f b3(8.0);
    ASSERT_EQ(a1 + a2, a3);
    
    return true;
}

TEST(VecDifference) {
    Vector<float, 16> a1(3.0);
    Vector<float, 16> a2(5.0);
    Vector<float, 16> a3(-2.0);
    ASSERT_EQ(a1 - a2, a3);

    // Vec4f Specialization
    Vec4f b1(3.0);
    Vec4f b2(5.0);
    Vec4f b3(-2.0);
    ASSERT_EQ(a1 - a2, a3);
    
    return true;
}

TEST(VecProduct) {
    Vector<float, 16> a1(3.0);
    Vector<float, 16> a2(5.0);
    Vector<float, 16> a3(15.0);
    ASSERT_EQ(a1 * a2, a3);

    // Vec4f Specialization
    Vec4f b1(3.0);
    Vec4f b2(5.0);
    Vec4f b3(15.0);
    ASSERT_EQ(a1 * a2, a3);
    
    return true;
}
 
TEST(VecDivision) {
    Vector<float, 16> a1(15.0);
    Vector<float, 16> a2(3.0);
    Vector<float, 16> a3(5.0);
    ASSERT_EQ(a1 / a2, a3);

    Vector<float, 16> b1(15.0);
    Vector<float, 16> b2(0.0);
    Vector<float, 16> b3(5.0);
    try {
        ASSERT_EQ(b1 / b2, b3);
        ASSERT_TRUE(false);
    } catch (std::runtime_error &e) {
        ASSERT_TRUE(true);
    }

    // Integer
    Vec2i c1(5);
    Vec2i c2(2);
    Vec2i c3(2);
    ASSERT_EQ(c1 / c2, c3);

    return true;
}


int main(){
    bool all_success = run_all_tests();
    return 0;
}

