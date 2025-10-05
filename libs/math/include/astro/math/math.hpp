#pragma once

#include <cstdint>
#include <initializer_list>
#include <optional>
#include <ostream>
#include <stdexcept>

namespace astro {
namespace math {
    
// --- Vector ------
template<typename T, int N>
struct Vector {
    T data[N];
    Vector() = default; // No initialization
    explicit Vector(const T val){     // Scalar initialization
        for(int i = 0; i < N; i++){
            data[i] = val;
        }
    };
    Vector(T& vector, int n){
        if(n != N) 
            throw std::runtime_error("Invalid dimensions for Vector constructor");
        for(int i = 0; i < N; i++) data[i] = vector[i];
    }
    Vector(const std::initializer_list<T> &list){ // List initialization
        if(N != list.size()) 
            throw std::runtime_error("Mismatch initializer list and vector lengths");
        int i = 0;
        for(auto it = list.begin(); it != list.end(); ++it, ++i){
            data[i] = *it;
        }
    }
    T& operator[](int i) { return data[i]; }
    const T& operator[](int i) const { return data[i]; }
};

template<typename T> struct Vector<T, 2> {
    union {
        T data[2]; 
        struct { T x, y; };
    };
    Vector() = default; // No initialization
    explicit Vector(const T val) : data(val, val) {} // Scalar fill
    Vector(T& vector, int n){ // Raw data initialization
        if(n != 2) 
            throw std::runtime_error("Invalid dimensions for Vector constructor");
        for(int i = 0; i < 2; i++) data[i] = vector[i];
    }
    Vector(const std::initializer_list<T> &list){ // List initialization
        if(list.size() != 2) 
            throw std::runtime_error("Mismatch initializer list and vector lengths");
        int i = 0;
        for(auto it = list.begin(); it != list.end(); ++it, ++i){ data[i] = *it; }
    }
    
    T& operator[](int i) { return data[i]; }
    const T& operator[](int i) const { return data[i]; }
};

template<typename T> struct Vector<T, 3> {
    union {
        T data[3]; 
        struct{ T x, y, z; };
    };
    Vector() = default; // No initialization
    explicit Vector(const T val) : data(val, val, val) {} // Scalar fill
    Vector(T& vector, int n){ // Raw data initialization
        if(n != 3) 
            throw std::runtime_error("Invalid dimensions for Vector constructor");
        for(int i = 0; i < 3; i++) data[i] = vector[i];
    }
    Vector(const std::initializer_list<T> &list){ // List initialization
        if(list.size() != 3) 
            throw std::runtime_error("Mismatch initializer list and vector lengths");
        int i = 0;
        for(auto it = list.begin(); it != list.end(); ++it, ++i){ data[i] = *it; }
    }
    
    T& operator[](int i) { return data[i]; }
    const T& operator[](int i) const { return data[i]; }
};

template<typename T> struct Vector<T, 4> {
    union {
        T data[4]; 
        struct { T x, y, z, w; };
        struct { T r, g, b, a; };
        Vector<T, 2> xy;
        Vector<T, 3> xyz;
        Vector<T, 3> rgb;
    };
    Vector() = default; // No initialization
    explicit Vector(const T val) : data(val, val, val, val) {} // scalar fill
    Vector(T& vector, int n){ // Raw data initialization
        if(n != 4) 
            throw std::runtime_error("Invalid dimensions for Vector constructor");
        for(int i = 0; i < 4; i++) data[i] = vector[i];
    }
    Vector(const std::initializer_list<T> &list){ // List initialization
        if(list.size() != 4) 
            throw std::runtime_error("Mismatch initializer list and vector lengths");
        int i = 0;
        for(auto it = list.begin(); it != list.end(); ++it, ++i){ data[i] = *it; }
    }
    
    T& operator[](int i) { return data[i]; }
    const T& operator[](int i) const { return data[i]; }
};

// Common type definitions
typedef Vector<float, 2> Vec2f;
typedef Vector<float, 3> Vec3f;
typedef Vector<float, 4> Vec4f;

typedef Vector<int, 2> Vec2i;

typedef Vector<uint8_t, 4> Color;
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



// --- Matrix ------

template<typename T, int N, int M> // N -> Rows | M -> Cols
struct Matrix {
    T data[N][M];
    Matrix() = default;
    explicit Matrix(const T& val){
        for(int j = 0; j < M; j++)
            for(int i = 0; i < N; i++)
                data[i][j] = val;
    }
    Matrix(const T (&matrix)[N][M], int n, int m){ // Raw data initialization
        if (n != N || m != M) 
            throw std::runtime_error("Invalid dimensions for matrix constructor");
        for(int j = 0; j < M; j++)
            for(int i = 0; i < N; i++)
                data[i][j] = matrix[i][j];
    }

};

template<typename T, int N, int M>
std::ostream& operator<<(std::ostream& os, const Matrix<T, N, M>& A) {
    os << N << 'x' << M << "_matrix";
    return os;
}

template <typename T, int N, int M>
bool operator==(Matrix<T, N, M>& A, Matrix<T, N, M>& B){
    bool equal = true;
    int i, j = 0;
    while (equal && i < N && j < M){
        equal = A.data[i][j] == B.data[i][j];
        i++; j++;
    }
    return equal;
}

}
}