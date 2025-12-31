
#include "astro/math/math.hpp"


namespace astro {
namespace math {

// ========================================================
// --- MATRIX ---------------------------------------------
// ========================================================
// Matrix * Vector specializations
template <>
Vec4f operator*(const Mat4f& A, const Vec4f& v) {
    return Vec4f(
        A(0,0)*v[0] + A(0,1)*v[1] + A(0,2)*v[2] + A(0,3)*v[3],
        A(1,0)*v[0] + A(1,1)*v[1] + A(1,2)*v[2] + A(1,3)*v[3],
        A(2,0)*v[0] + A(2,1)*v[1] + A(2,2)*v[2] + A(2,3)*v[3],
        A(3,0)*v[0] + A(3,1)*v[1] + A(3,2)*v[2] + A(3,3)*v[3]
    );
}

// Matrix inverse specializations
template <>
inline Mat3f inverse(const Mat3f& m) {
    float det = determinant(m);

    // This is a naive check. Singular matrices should be handled in a better way
    if (std::abs(det) < 1e-9) throw std::runtime_error("Matrix is singular");
    float invDet = 1.0f / det;

    Mat3f res(0.0f);
    res(0, 0) = (m(1, 1) * m(2, 2) - m(1, 2) * m(2, 1)) * invDet;
    res(0, 1) = (m(0, 2) * m(2, 1) - m(0, 1) * m(2, 2)) * invDet;
    res(0, 2) = (m(0, 1) * m(1, 2) - m(0, 2) * m(1, 1)) * invDet;
    res(1, 0) = (m(1, 2) * m(2, 0) - m(1, 0) * m(2, 2)) * invDet;
    res(1, 1) = (m(0, 0) * m(2, 2) - m(0, 2) * m(2, 0)) * invDet;
    res(1, 2) = (m(1, 0) * m(0, 2) - m(0, 0) * m(1, 2)) * invDet;
    res(2, 0) = (m(1, 0) * m(2, 1) - m(1, 1) * m(2, 0)) * invDet;
    res(2, 1) = (m(2, 0) * m(0, 1) - m(0, 0) * m(2, 1)) * invDet;
    res(2, 2) = (m(0, 0) * m(1, 1) - m(1, 0) * m(0, 1)) * invDet;
    return res;
}
template <>
inline Mat4f inverse(const Mat4f& m) {
    // Determinant 4x4 using 2x2 sub-determinants (Laplace Expansion)
    float s0 = m(0,0) * m(1,1) - m(0,1) * m(1,0);
    float s1 = m(0,0) * m(1,2) - m(0,2) * m(1,0);
    float s2 = m(0,0) * m(1,3) - m(0,3) * m(1,0);
    float s3 = m(0,1) * m(1,2) - m(0,2) * m(1,1);
    float s4 = m(0,1) * m(1,3) - m(0,3) * m(1,1);
    float s5 = m(0,2) * m(1,3) - m(0,3) * m(1,2);

    float c5 = m(2,2) * m(3,3) - m(2,3) * m(3,2);
    float c4 = m(2,1) * m(3,3) - m(2,3) * m(3,1);
    float c3 = m(2,1) * m(3,2) - m(2,2) * m(3,1);
    float c2 = m(2,0) * m(3,3) - m(2,3) * m(3,0);
    float c1 = m(2,0) * m(3,2) - m(2,2) * m(3,0);
    float c0 = m(2,0) * m(3,1) - m(2,1) * m(3,0);

    float det = (s0 * c5 - s1 * c4 + s2 * c3 + s3 * c2 - s4 * c1 + s5 * c0);
    // This is a naive check. Singular matrices should be handled in a better way
    if (std::abs(det) < 1e-9) throw std::runtime_error("Matrix is singular");

    float invDet = 1.0f / det;

    // Inverse matrix
    Mat4f res(0.0f);
    res(0,0) = ( m(1,1) * c5 - m(1,2) * c4 + m(1,3) * c3) * invDet;
    res(0,1) = (-m(0,1) * c5 + m(0,2) * c4 - m(0,3) * c3) * invDet;
    res(0,2) = ( m(3,1) * s5 - m(3,2) * s4 + m(3,3) * s3) * invDet;
    res(0,3) = (-m(2,1) * s5 + m(2,2) * s4 - m(2,3) * s3) * invDet;

    res(1,0) = (-m(1,0) * c5 + m(1,2) * c2 - m(1,3) * c1) * invDet;
    res(1,1) = ( m(0,0) * c5 - m(0,2) * c2 + m(0,3) * c1) * invDet;
    res(1,2) = (-m(3,0) * s5 + m(3,2) * s2 - m(3,3) * s1) * invDet;
    res(1,3) = ( m(2,0) * s5 - m(2,2) * s2 + m(2,3) * s1) * invDet;

    res(2,0) = ( m(1,0) * c4 - m(1,1) * c2 + m(1,3) * c0) * invDet;
    res(2,1) = (-m(0,0) * c4 + m(0,1) * c2 - m(0,3) * c0) * invDet;
    res(2,2) = ( m(3,0) * s4 - m(3,1) * s2 + m(3,3) * s0) * invDet;
    res(2,3) = (-m(2,0) * s4 + m(2,1) * s2 - m(2,3) * s0) * invDet;

    res(3,0) = (-m(1,0) * c3 + m(1,1) * c1 - m(1,2) * c0) * invDet;
    res(3,1) = ( m(0,0) * c3 - m(0,1) * c1 + m(0,2) * c0) * invDet;
    res(3,2) = (-m(3,0) * s3 + m(3,1) * s1 - m(3,2) * s0) * invDet;
    res(3,3) = ( m(2,0) * s3 - m(2,1) * s1 + m(2,2) * s0) * invDet;

    return res;
}

}
}