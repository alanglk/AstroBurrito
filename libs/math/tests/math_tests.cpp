#include "astro/math/math.hpp"
#include "astro_test.hpp"
#include <stdexcept>

using namespace astro::math;

// ========================================================
// --- VECTOR ---------------------------------------------
// ========================================================
TEST(VecInitialization) {
    Vector<int, 5> a1(5);
    for(int i = 0; i < 5; i++) ASSERT_EQ(a1[i], 5);
    Vector<int, 5> a2({0, 1, 2, 3, 4});
    for(int i = 0; i < 5; i++) ASSERT_EQ(a2[i], i);
    Vector<int, 5> a3 = a2; // Copy-constructor
    for(int i = 0; i < 5; i++) ASSERT_EQ(a3[i], a2[i]);
    Vector<int, 5> a4 = {0, 1, 2, 3, 4};
    for(int i = 0; i < 5; i++) ASSERT_EQ(a4[i], i);
    
    // Vec2f Specialization
    Vec2f b1(5.0);
    for(int i = 0; i < 2; i++) ASSERT_EQ(b1[i], 5.0);
    Vec2f b2({0.0, 1.0});
    for(int i = 0; i < 2; i++) ASSERT_EQ(b2[i], float(i));
    Vec2f b3 = b2; // Copy-constructor
    for(int i = 0; i < 2; i++) ASSERT_EQ(b3[i], b2[i]);
    Vec2f b4 = {0.0, 1.0};
    for(int i = 0; i < 2; i++) ASSERT_EQ(b4[i], i);
    
    // Vec3f Specialization
    Vec3f c1(5.0);
    for(int i = 0; i < 3; i++) ASSERT_EQ(c1[i], 5.0);
    Vec3f c2({0.0, 1.0, 2.0});
    for(int i = 0; i < 3; i++) ASSERT_EQ(c2[i], float(i));
    Vec3f c3 = c2; // Copy-constructor
    for(int i = 0; i < 3; i++) ASSERT_EQ(c3[i], c2[i]);
    Vec3f c4 = {0, 1, 2};
    for(int i = 0; i < 3; i++) ASSERT_EQ(c4[i], i);
    
    // Vec4f Specialization
    Vec4f d1(5.0);
    for(int i = 0; i < 4; i++) ASSERT_EQ(d1[i], 5.0);
    Vec4f d2({0.0, 1.0, 2.0, 3.0});
    for(int i = 0; i < 4; i++) ASSERT_EQ(d2[i], float(i));
    Vec4f d3 = d2; // Copy-constructor
    for(int i = 0; i < 4; i++) ASSERT_EQ(d3[i], d2[i]);
    Vec4f d4 = {0.0, 1.0, 2.0, 3.0};
    for(int i = 0; i < 4; i++) ASSERT_EQ(d4[i], i);
    
    return true;
}

TEST(VecAccess){
    Vector<int, 5> a1({1, 2, 3, 4, 5});
    ASSERT_EQ(a1[0], 1);
    ASSERT_EQ(a1[1], 2);
    ASSERT_EQ(a1[2], 3);
    ASSERT_EQ(a1[3], 4);
    ASSERT_EQ(a1[4], 5);

    Vec2f b1({1.0, 2.0});
    ASSERT_EQ(b1[0], 1.0);
    ASSERT_EQ(b1[1], 2.0);

    Vec3f c1({1.0, 2.0, 3.0});
    ASSERT_EQ(c1[0], 1.0);
    ASSERT_EQ(c1[1], 2.0);
    ASSERT_EQ(c1[2], 3.0);

    Vec4f d1({1.0, 2.0, 3.0, 4.0});
    ASSERT_EQ(d1[0], 1.0);
    ASSERT_EQ(d1[1], 2.0);
    ASSERT_EQ(d1[2], 3.0);
    ASSERT_EQ(d1[3], 4.0);
    return true;
}

TEST(VecAssignment){
    Vector<int, 5> a1(5);
    for(int i = 0; i < 5; i++) ASSERT_EQ(a1[i], 5);
    a1 = 1; // Implicit fill assignment
    for(int i = 0; i < 5; i++) ASSERT_EQ(a1[i], 1);
    
    Vector<int, 5> a2(3);
    for(int i = 0; i < 5; i++) ASSERT_EQ(a2[i], 3);
    a2 = a1; // Copy assigment
    for(int i = 0; i < 5; i++) ASSERT_EQ(a2[i], a1[i]);

    return true;
}

TEST(VecEquality) {
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

TEST(VecLen){
    return false;
}


// ========================================================
// --- MATRIX ---------------------------------------------
// ========================================================

TEST(MatrixInitialization){
    Matrix<int, 3, 3> A2(5); // Const value initialization
    for(int j = 0; j < 3; j++){
        for(int i = 0; i < 3; i++){
            ASSERT_EQ(A2(j, i), 5);
        }
    }
    Matrix<int, 3, 3> A3({0, 1, 2, 3, 4, 5, 6, 7, 8}); // List initialization
    for(int j = 0; j < 3; j++){
        for(int i = 0; i < 3; i++){
            ASSERT_EQ(A3(j, i), i+3*j);
        }
    }
    int A4_raw[3][3] = {{0, 1, 2}, {3, 4, 5}, {6, 7, 8}};
    Matrix<int, 3, 3> A4(A4_raw); // 2D array
    for(int j = 0; j < 3; j++){
        for(int i = 0; i < 3; i++){
            ASSERT_EQ(A4(j, i), i+3*j);
        }
    }
    
    return true;
}

TEST(MatrixViewInitialization){
    int A1_raw[9] = {0, 1, 2, 3, 4, 5, 6, 7, 8};
    Matrix_View<int, 3, 3> A1(A1_raw);
    for(int j = 0; j < 3; j++){
        for(int i = 0; i < 3; i++){
            ASSERT_EQ(A1(j, i), i+3*j);
        }
    }
    
    return true;
}

TEST(MatrixEquality){
    Matrix<int, 3, 3> A1(1);
    Matrix<int, 3, 3> A2(1);
    Matrix<int, 3, 3> A3({0, 1, 2, 3, 4, 5, 6, 7, 8}); // List initialization
    return A1 == A2;
    ASSERT_EQ(A1, A2);
    ASSERT_NEQ(A1, A3);

    int B1_raw[9] = {0, 1, 2, 3, 4, 5, 6, 7, 8};
    int B3_raw[9] = {8, 7, 6, 5, 4, 3, 2, 1 ,0};
    Matrix_View<int, 3, 3> B1(B1_raw);
    Matrix_View<int, 3, 3> B2(B1_raw);
    Matrix_View<int, 3, 3> B3(B3_raw);
    ASSERT_EQ(B1, B2);
    ASSERT_NEQ(B1, B3);
    for(int j = 0; j < 3; j++){
        for(int i = 0; i < 3; i++){
            ASSERT_EQ(B1(j, i), i+3*j);
            ASSERT_EQ(B3(j, i), 8 -i+3*j);
            ASSERT_EQ(B1(j, i), B3(2 - j, 2 - i));
        }
    }
    
    ASSERT_EQ(A3, B1);
}


int main(){
    bool all_success = run_all_tests();
    return !all_success;
}

