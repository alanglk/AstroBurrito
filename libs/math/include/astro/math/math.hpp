#pragma once

#include <algorithm>
#include <array>
#include <cstring>
#include <initializer_list>
#include <ostream>
#include <stdexcept>

namespace astro {
namespace math {
    
// ========================================================
// --- VECTOR ---------------------------------------------
// ========================================================
template<typename T, int N>
struct Vector {
    using value_type = T;
    static constexpr int rows = N;
    static constexpr int cols = 1;

    T data[N];
    Vector() = default; // No initialization
    explicit Vector(const T& val){     // Scalar initialization
        std::fill(data, data+N, val);
    };
    Vector(Vector<T, N>& vec){ // Copy-Constructor
        for (int i = 0; i < N; i++) data[i] = vec[i];
    }
    Vector(const std::initializer_list<T> &list){ // List initialization
        if(N != list.size()) 
            throw std::runtime_error("Mismatch initializer list and vector lengths");
        int i = 0;
        for(auto it = list.begin(); it != list.end(); ++it, ++i){
            data[i] = *it;
        }
    }

    // Access operators
    T& operator[](int i) { return data[i]; }
    const T& operator[](int i) const { return data[i]; }
    
    // Other matrix-like access operators
    T& operator()(int r, int c = 0) {
        if (c != 0 || r < 0 || r >= N) throw std::out_of_range("Vector index out of bounds");
        return data[r];
    }
    const T& operator()(int r, int c = 0) const {
        if (c != 0 || r < 0 || r >= N) throw std::out_of_range("Vector index out of bounds");
        return data[r];
    }
    
    // Assignment operators
    Vector<T, N>& operator=(const T& val){
        std::fill(data, data+N, val);
        return *this;
    }
};

template<typename T> struct Vector<T, 2> {
    using value_type = T;
    static constexpr int rows = 2;
    static constexpr int cols = 1;

    union {
        T data[2]; 
        struct { T x, y; };
    };
    Vector() = default; // No initialization
    Vector(const T& val) : data(val, val) {} // Scalar fill
    Vector(const Vector<T, rows>& vec){ // Copy-Constructor
        for (int i = 0; i < rows; i++) data[i] = vec[i];
    }
    Vector(T x, T y): data(x, y) {};
    Vector(const std::initializer_list<T> &list){ // List initialization
        if(list.size() != rows) 
            throw std::runtime_error("Mismatch initializer list and vector lengths");
        int i = 0;
        for(auto it = list.begin(); it != list.end(); ++it, ++i){ data[i] = *it; }
    }
    
    // Access operators
    T& operator[](int i) { return data[i]; }
    const T& operator[](int i) const { return data[i]; }

    // Other matrix-like access operators
    T& operator()(int r, int c = 0) {
        if (c != 0 || r < 0 || r >= rows) throw std::out_of_range("Vector index out of bounds");
        return data[r];
    }
    const T& operator()(int r, int c = 0) const {
        if (c != 0 || r < 0 || r >= rows) throw std::out_of_range("Vector index out of bounds");
        return data[r];
    }

    // Assignment operators
    Vector<T, rows>& operator=(const T& val){
        std::fill(data, data+rows, val);
        return *this;
    }
};

template<typename T> struct Vector<T, 3> {
    using value_type = T;
    static constexpr int rows = 3;
    static constexpr int cols = 1;

    union {
        T data[3]; 
        struct{ T x, y, z; };
        struct { T r, g, b; };
    };
    Vector() = default; // No initialization
    Vector(const T& val) : data(val, val, val) {} // Scalar fill
    Vector(const Vector<T, rows>& vec){ // Copy-Constructor
        for (int i = 0; i < rows; i++) data[i] = vec[i];
    }
    Vector(T x, T y, T z): data(x, y, z) {};
    Vector(const std::initializer_list<T> &list){ // List initialization
        if(list.size() != rows) 
            throw std::runtime_error("Mismatch initializer list and vector lengths");
        int i = 0;
        for(auto it = list.begin(); it != list.end(); ++it, ++i){ data[i] = *it; }
    }
    
    // Access operators
    T& operator[](int i) { return data[i]; }
    const T& operator[](int i) const { return data[i]; }

    // Other matrix-like access operators
    T& operator()(int r, int c = 0) {
        if (c != 0 || r < 0 || r >= rows) throw std::out_of_range("Vector index out of bounds");
        return data[r];
    }
    const T& operator()(int r, int c = 0) const {
        if (c != 0 || r < 0 || r >= rows) throw std::out_of_range("Vector index out of bounds");
        return data[r];
    }

    // Assignment operators
    Vector<T, rows>& operator=(const T& val){
        std::fill(data, data+rows, val);
        return *this;
    }
};

template<typename T> struct Vector<T, 4> {
    using value_type = T;
    static constexpr int rows = 4;
    static constexpr int cols = 1;

    union {
        T data[4]; 
        struct { T x, y, z, w; };
        struct { T r, g, b, a; };
        Vector<T, 2> xy;
        Vector<T, 3> xyz;
        Vector<T, 3> rgb;
    };
    Vector() = default; // No initialization
    Vector(const T& val) : data(val, val, val, val) {} // Scalar fill
    Vector(const Vector<T, rows>& vec){ // Copy-Constructor
        for (int i = 0; i < rows; i++) data[i] = vec[i];
    }
    Vector(T x, T y, T z, T w): data(x, y, z, w) {};
    Vector(const std::initializer_list<T> &list){ // List initialization
        if(list.size() != rows) 
            throw std::runtime_error("Mismatch initializer list and vector lengths");
        int i = 0;
        for(auto it = list.begin(); it != list.end(); ++it, ++i){ data[i] = *it; }
    }
    
    // Access operators
    T& operator[](int i) { return data[i]; }
    const T& operator[](int i) const { return data[i]; }

    // Other matrix-like access operators
    T& operator()(int r, int c = 0) {
        if (c != 0 || r < 0 || r >= rows) throw std::out_of_range("Vector index out of bounds");
        return data[r];
    }
    const T& operator()(int r, int c = 0) const {
        if (c != 0 || r < 0 || r >= rows) throw std::out_of_range("Vector index out of bounds");
        return data[r];
    }

    // Assignment operators
    Vector<T, rows>& operator=(const T& val){
        std::fill(data, data+rows, val);
        return *this;
    }
};

// Common type definitions
typedef Vector<float, 2> Vec2f;
typedef Vector<float, 3> Vec3f;
typedef Vector<float, 4> Vec4f;

typedef Vector<int, 2> Vec2i;

typedef Vector<uint, 4> Color32;


// Vector Operators
template<typename T, int N>
std::ostream& operator<<(std::ostream& os, const Vector<T, N>& a) {
    os << '[';
    int n_1 = N -1;
    for(int i = 0; i<N; i++){
       os << a.data[i]; 
       if(i != (n_1)) os << ", ";
    }
    os << ']';
    return os;
}

template<typename T, int N>
bool operator==(const Vector<T, N>& a, const Vector<T, N>& b) {
    bool equal = true; int i = 0;
    while(equal && i < N){
        equal = a.data[i] == b.data[i];
        i++;
    }
    return equal; 
}
template<typename T, int N>
bool operator!=(const Vector<T, N>& a, const Vector<T, N>& b) {
    return !(a == b); 
}

/**
 * @brief Element wise vector sum
 * @return Vector<T, N> summed vector
 */
template<typename T, int N>
Vector<T, N> operator+(const Vector<T, N>& a, const Vector<T, N>& b) {
    Vector<T, N> res; 
    for(int i = 0; i < N; i++) res.data[i] = a.data[i] + b.data[i];
    return res; 
}

/**
 * @brief Element wise vector difference
 * @return Vector<T, N> difference vector
 */
template<typename T, int N>
Vector<T, N> operator-(const Vector<T, N>& a, const Vector<T, N>& b) {
    Vector<T, N> res; 
    for(int i = 0; i < N; i++) res.data[i] = a.data[i] - b.data[i];
    return res; 
}

/**
 * @brief Element wise vector product
 * @return Vector<T, N> elm-product vector
 */
template<typename T, int N>
Vector<T, N> operator*(const Vector<T, N>& a, const Vector<T, N>& b) {
    Vector<T, N> res; 
    for(int i = 0; i < N; i++) res.data[i] = a.data[i] * b.data[i];
    return res; 
}

/**
 * @brief Element wise vector division
 * @return Vector<T, N> elm-division vector
 */
template<typename T, int N>
Vector<T, N> operator/(const Vector<T, N>& a, const Vector<T, N>& b) {
    Vector<T, N> res; 
    for(int i = 0; i < N; i++) {
        if(b.data[i] == 0.0) throw std::runtime_error("Division by 0");
        res.data[i] = a.data[i] / b.data[i];
    }
    return res; 
}

/**
 * @brief Dot product between two vectors
 * @return T
 */
template<typename T, int N>
T dot(const Vector<T, N>& a, const Vector<T, N>& b) {
    T res; 
    for(int i = 0; i < N; i++) res += a.data[i] * b.data[i];
    return res; 
}

/**
 * @brief Get the dimension of a vector
 * @return int 
 */
template<typename T, int N>
int len(Vector<T, N>){ return N; }


// ========================================================
// --- MATRIX ---------------------------------------------
// ========================================================
template<typename T, int N, int M> // N -> Rows | M -> Cols
struct Matrix {
    std::array<T, N*M> data; 
    using value_type = T;
    static constexpr int rows = N;
    static constexpr int cols = M;

    // Helper function to calculate the 1D index from 2D coordinates (row-major order)
    constexpr int index(int r, int c) const {
        if (r < 0 || r >= N || c < 0 || c >= M) {
            throw std::out_of_range("Matrix index out of bounds.");
        }
        return r * M + c;
    }
    
    Matrix() = delete;
    // Uniform Initialization Constructor
    explicit Matrix(const T& val) {
            std::fill(data.begin(), data.end(), val);
    }
    Matrix(const Matrix<T, rows, cols>& mat){ // Copy-Constructor
        std::memcpy(this->data.data(), mat.data.data(), rows*cols);
    }
    Matrix(const std::initializer_list<T> &list){ // List initialization
        if(N*M != list.size()) 
            throw std::runtime_error("Mismatch initializer list and vector lengths");
        int i = 0;
        for(auto it = list.begin(); it != list.end(); ++it, ++i){
            data[i] = *it;
        }
    }
    // Copy from a C-style 2D array
    Matrix(const T (&matrix)[N][M]) {
        for(int r = 0; r < N; ++r) {
            for(int c = 0; c < M; ++c) {
                data[index(r, c)] = matrix[r][c];
            }
        }
    }
    
    // Access operator: Matrix[i][j]
    T& operator()(int r, int c) {
        return data[index(r, c)];
    }
    const T& operator()(int r, int c) const {
        return data[index(r, c)];
    }

    // Static identity matrix creation
    static Matrix<T, N, M> Identity(const T& diag_val = static_cast<T>(1)) {
        static_assert(N == M, "Identity matrix must be square");
        Matrix<T, N, M> res(static_cast<T>(0));
        for (int i = 0; i < N; ++i)
            res(i, i) = diag_val;
        return res;
    }
    

};

typedef Matrix<float, 4, 4> Mat4f;

template<typename T, int N, int M>
struct Matrix_View {
    T* data;  // Non-owning Pointer
    using value_type = T;
    static constexpr int rows = N;
    static constexpr int cols = M;

    // Helper function to calculate the 1D index from 2D coordinates (row-major order)
    constexpr int index(int r, int c) const {
        if (r < 0 || r >= N || c < 0 || c >= M) {
            throw std::out_of_range("Matrix index out of bounds.");
        }
        return r * M + c;
    }

    // Takes a raw pointer to external data.
    explicit Matrix_View(T* external_data) : data(external_data) {
        if (data == nullptr) {
            throw std::invalid_argument("Matrix view requires non-null pointer.");
        }
    }
    
    // Deleted default constructor (must initialize with data)
    Matrix_View() = delete; 
    // **Integration Point:** Constructor to create a view from an owning matrix.
    explicit Matrix_View(Matrix<T, N, M>& other) : data(other.data.data()) {}

    // Access operators
    T& operator()(int r, int c) {
        return data[index(r, c)];
    }
    const T& operator()(int r, int c) const {
        return data[index(r, c)];
    }
    
};

// Matrix concept
template<typename M>
concept MatrixLike = requires(const M& m, int r, int c) {
    // It must have these member types (or use std::remove_cvref_t<decltype(m(r,c))>)
    typename M::value_type; 
    
    // It must have these static members for size
    { M::rows } -> std::convertible_to<int>;
    { M::cols } -> std::convertible_to<int>;

    // It must have a const-qualified access operator
    { m(r, c) } -> std::convertible_to<const typename M::value_type&>;
};

// Matrix operators
template <MatrixLike Mat>
std::ostream& operator<<(std::ostream& os, const Mat& A) {
    os << "Matrix_" << Mat::rows << "x" << Mat::cols << ":\n";
    if (Mat::rows > 4 || Mat::cols > 4) return os;
    for (int r = 0; r < Mat::rows; ++r) {
        for (int c = 0; c < Mat::cols; ++c)
            os << A(r, c) << " ";
        os << "\n";
    }
    return os;
}
template <MatrixLike MatA, MatrixLike MatB>
requires (MatA::rows == MatB::rows && MatA::cols == MatB::cols)
bool operator==(const MatA& A, const MatB& B) {
    for (int r = 0; r < MatA::rows; ++r)
        for (int c = 0; c < MatA::cols; ++c)
            if (A(r, c) != B(r, c))
                return false;
    return true;
}
template <MatrixLike MatA, MatrixLike MatB>
requires (MatA::rows == MatB::rows && MatA::cols == MatB::cols)
bool operator!=(const MatA& A, const MatB& B) {
    return !(A == B);
}

// Matrix * Matrix
template <MatrixLike MatA, MatrixLike MatB>
requires (MatA::cols == MatB::rows)
Matrix<typename MatA::value_type, MatA::rows, MatB::cols>
operator*(const MatA& A, const MatB& B) {
    using T = typename MatA::value_type;
    Matrix<T, MatA::rows, MatB::cols> res(static_cast<T>(0));

    for (int i = 0; i < MatA::rows; ++i)
        for (int j = 0; j < MatB::cols; ++j)
            for (int k = 0; k < MatA::cols; ++k)
                res(i, j) += A(i, k) * B(k, j);
    return res;
}


// Matrix * Vector
template <MatrixLike MatA, typename T, int N>
requires (MatA::cols == N)
Vector<T, MatA::rows> operator*(const MatA& A, const Vector<T, N>& v) {
    Vector<T, MatA::rows> res(static_cast<T>(0));
    for (int i = 0; i < MatA::rows; ++i)
        for (int j = 0; j < MatA::cols; ++j)
            res[i] += A(i, j) * v[j];
    return res;
}

// Vector * Matrix
template <typename T, int N, MatrixLike MatB>
requires (MatB::rows == N)
Vector<T, MatB::cols> operator*(const Vector<T, N>& v, const MatB& B) {
    Vector<T, MatB::cols> res(static_cast<T>(0));
    for (int j = 0; j < MatB::cols; ++j)
        for (int k = 0; k < MatB::rows; ++k)
            res[j] += v[k] * B(k, j);
    return res;
}


}
}