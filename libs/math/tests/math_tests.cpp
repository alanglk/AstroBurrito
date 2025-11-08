#include "astro/math/math.hpp"
#include "astro_test.hpp"

#include <cmath>
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
    
    // TODO: ADD SCALAR SUM TEST
    return false;
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
    
    // TODO: ADD SCALAR DIFF TEST
    return false;
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
    
    // TODO: ADD SCALAR PROD TEST
    return false;
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

    // TODO: ADD SCALAR DIV TEST
    return false;
}

TEST(VecLen){
    Vector<float, 16> a1(15.0);
    Vec2i a2(3);
    Vec3f a3(0.0); 
    Vec4f a4(3.0);
    
    ASSERT_EQ(16, len(a1));
    ASSERT_EQ(2, len(a2));
    ASSERT_EQ(3, len(a3));
    ASSERT_EQ(4, len(a4));

    return true;
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
    ASSERT_EQ(A1, A2);
    ASSERT_NEQ(A1, A3);

    int B1_raw[9] = {0, 1, 2, 3, 4, 5, 6, 7, 8};
    int B3_raw[9] = {8, 7, 6, 5, 4, 3, 2, 1, 0};

    Matrix_View<int, 3, 3> B1(B1_raw);
    Matrix_View<int, 3, 3> B2(B1_raw);
    Matrix_View<int, 3, 3> B3(B3_raw);

    ASSERT_EQ(B1, B2);
    ASSERT_NEQ(B1, B3);

    for (int r = 0; r < B1.rows; ++r) {
        for (int c = 0; c < B1.cols; ++c) {
            ASSERT_EQ(B1(r, c), c + r * B1.cols);
            ASSERT_EQ(B3(r, c), 8 - (c + r * B1.cols));
            ASSERT_EQ(B3(r, c), B1(2 - r, 2 - c));  // correct symmetric mirror
        }
    }
    
    ASSERT_EQ(A3, B1);
    return true;
}

TEST(MatrixMultiplication){
    // Basic matrix multiplication
    Matrix<int, 2, 2> A1(1);
    Matrix<int, 2, 2> A2(2);
    Matrix<int, 2, 2> A_expect(4);
    ASSERT_EQ(A1*A2, A_expect);
    
    
    // Non-trivial values
    Matrix<int, 2, 3> B1({1, 2, 3, 4, 5, 6});
    Matrix<int, 3, 2> B2({7, 8, 9, 10, 11, 12});
    Matrix<int, 2, 2> B_expect({ 58, 64, 139, 154});
    ASSERT_EQ(B1 * B2, B_expect);

    // Identity matrix
    Matrix<int, 3, 3> I({1,0,0, 0,1,0, 0,0,1});
    Matrix<int, 3, 3> M({2,3,4, 5,6,7, 8,9,10});
    ASSERT_EQ(M * I, M);
    ASSERT_EQ(I * M, M);

    // Zero matrix
    Matrix<int, 2, 3> Z(0);
    Matrix<int, 3, 4> N({1,2,3,4,5,6,7,8, 9,10,11,12});
    Matrix<int, 2, 4> Z_expect(0);
    ASSERT_EQ(Z * N, Z_expect);
 
    // Matrix-Vector multiplication
    Matrix<int, 3, 3> Mv({1,2,3, 4,5,6, 7,8,9});
    Vector<int, 3> v({1, 0, 1});   // column vector
    Vector<int, 3> v_expect({4, 10, 16});
    ASSERT_EQ(Mv * v, v_expect);
    
    // Vector-Matrix multiplication
    Vector<int, 3> v_row({1, 2, 3});
    Matrix<int, 3, 2> Mv2({1,2, 3,4, 5,6});
    Vector<int, 2> expect_row({22, 28});
    ASSERT_EQ(v_row * Mv2, expect_row);
    
    // Matrix-view interoperability
    int rawA[6] = {1,2,3,4,5,6};
    int rawB[6] = {7,8,9,10,11,12};
    Matrix_View<int, 2, 3> A_view(rawA);
    Matrix_View<int, 3, 2> B_view(rawB);
    Matrix<int, 2, 2> expect({ 58, 64, 139, 154 });
    ASSERT_EQ(A_view * B_view, expect);
    
    // Const correctness
    const Matrix<float, 2, 2> Af({1.5f, 2.0f, 3.0f, 4.5f});
    const Matrix<float, 2, 2> Bf({2.0f, 0.0f, 1.0f, 2.0f});
    Matrix<float, 2, 2> expect_f({ 5.0f, 4.0f, 10.5f, 9.0f });
    ASSERT_EQ(Af * Bf, expect_f); 
    
    // 1xN * Nx1
    Matrix<int, 1, 3> A1x3({1,2,3});
    Matrix<int, 3, 1> A3x1({4,5,6});
    Matrix<int, 1, 1> scalar_expect({32}); // 1*4 + 2*5 + 3*6
    ASSERT_EQ(A1x3 * A3x1, scalar_expect);
    
    // Self multiplication
    Matrix<int, 2, 2> Mx({1, 2, 3, 4});
    Matrix<int, 2, 2> Mx2({7, 10, 15, 22});
    ASSERT_EQ(Mx * Mx, Mx2);

    return true;
}


TEST(MatrixProjection) {

    // --- 1. Define a simple perspective projection matrix ---
    // fov = 90°, aspect = 1, near = 1, far = 10
    float fov = 90.0f * (3.1415926f / 180.0f);
    float aspect = 1.0f;
    float near = 1.0f;
    float far = 10.0f;

    float f = 1.0f / std::tan(fov / 2.0f);
    Mat4f P({
        f / aspect, 0, 0, 0,
        0, f, 0, 0,
        0, 0, (far + near) / (near - far), (2 * far * near) / (near - far),
        0, 0, -1, 0
    });

    // --- 2. Define some 3D points in homogeneous coordinates ---
    Vec4f point1({0, 0, -5, 1});    // Center
    Vec4f point2({1, 1, -5, 1});    // Top-right
    Vec4f point3({-1, -1, -5, 1});  // Bottom-left

    // --- 3. Multiply by projection matrix ---

    Vec4f proj1 = P * point1;
    Vec4f proj2 = P * point2;
    Vec4f proj3 = P * point3;

    // --- 4. Perspective divide (x/w, y/w, z/w) ---
    Vec3f ndc1({proj1[0]/proj1[3], proj1[1]/proj1[3], proj1[2]/proj1[3]});
    Vec3f ndc2({proj2[0]/proj2[3], proj2[1]/proj2[3], proj2[2]/proj2[3]});
    Vec3f ndc3({proj3[0]/proj3[3], proj3[1]/proj3[3], proj3[2]/proj3[3]});

    // --- 5. Assertions (approximate floating-point comparison) ---
    auto eq = [](float a, float b) { return std::fabs(a - b) < 1e-4f; };

    // Center should remain at center (0,0)
    ASSERT_TRUE(eq(ndc1[0], 0.0f));
    ASSERT_TRUE(eq(ndc1[1], 0.0f));

    // Points on same depth → same z NDC (normalized)
    ASSERT_TRUE(eq(ndc1[2], ndc2[2]));
    ASSERT_TRUE(eq(ndc1[2], ndc3[2]));

    // Right and top points should project positively
    ASSERT_TRUE(ndc2[0] > 0.0f);
    ASSERT_TRUE(ndc2[1] > 0.0f);

    // Left and bottom points should project negatively
    ASSERT_TRUE(ndc3[0] < 0.0f);
    ASSERT_TRUE(ndc3[1] < 0.0f);

    return true;
}

int main(){
    bool all_success = run_all_tests();
    return !all_success;
}

