#ifndef MATH_MAT3_HPP
#define MATH_MAT3_HPP

struct mat3 {
  float m[9];

  constexpr mat3();
  constexpr mat3(float a,
                 float b,
                 float c,
                 float d,
                 float e,
                 float f,
                 float g,
                 float h,
                 float i);

  constexpr float operator[](int n);

  operator float*();
  float& operator[](int n);
  mat3& operator+=(const mat3& n);
  mat3& operator+=(float s);
  mat3& operator-=(const mat3& n);
  mat3& operator-=(float s);
  mat3& operator*=(float s);
  mat3& operator/=(float s);
  mat3& operator=(const mat3& n);

  mat3& identity();
  mat3& transpose();
  mat3& inverse();
};

constexpr mat3 operator+(const mat3& m, float s);
constexpr mat3 operator+(float s, const mat3& m);
constexpr mat3 operator-(const mat3& m, float s);
constexpr mat3 operator-(const mat3& m);
constexpr mat3 operator-(float s, const mat3& m);
constexpr vec3 operator*(const mat3& m, const vec3& v);
constexpr mat3 operator*(const mat3& m, const mat3& n);
constexpr mat3 operator*(float s, const mat3& m);
constexpr mat3 operator*(const mat3& m, float s);
constexpr mat3 operator/(const mat3& m, float s);
constexpr bool operator==(const mat3& m, const mat3& n);
constexpr bool operator!=(const mat3& m, const mat3& n);

// matrix functions
constexpr float determinant(const mat3& m);
constexpr mat3 transpose(const mat3& m);
constexpr mat3 inverse(const mat3& m);

// affine matrices
constexpr mat3 identity3();

#endif