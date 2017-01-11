#ifndef MATH_VEC2_HPP
#define MATH_VEC2_HPP

struct vec2 {

  union {
    float v[2];

#include "vec2_swizzle.tpp"
  };

  constexpr vec2();
  explicit constexpr vec2(float x);
  explicit constexpr vec2(const float* u);
  constexpr vec2(float x, float y);
  explicit constexpr vec2(const vec3& u);
  explicit constexpr vec2(const vec4& u);

  constexpr float operator[](int n);

  operator float*();
  float& operator[](int n);
  vec2& operator+=(float s);
  vec2& operator+=(const vec2& u);
  vec2& operator-=(float s);
  vec2& operator-=(const vec2& u);
  vec2& operator*=(float s);
  vec2& operator*=(const vec2& u);
  vec2& operator/=(float s);
  vec2& operator/=(const vec2& u);
  vec2& operator=(const vec2& u);
  vec2& normalize();
};

constexpr vec2 operator+(float s, const vec2& v);
constexpr vec2 operator+(const vec2& v, float s);
constexpr vec2 operator+(const vec2& v, const vec2& u);

constexpr vec2 operator-(const vec2& v);
constexpr vec2 operator-(float s, const vec2& v);
constexpr vec2 operator-(const vec2& v, float s);
constexpr vec2 operator-(const vec2& v, const vec2& u);

constexpr vec2 operator*(float s, const vec2& v);
constexpr vec2 operator*(const vec2& v, float s);
constexpr vec2 operator*(const vec2& v, const vec2& u);

constexpr vec2 operator/(float s, const vec2& v);
constexpr vec2 operator/(const vec2& v, float s);
constexpr vec2 operator/(const vec2& v, const vec2& u);

constexpr bool operator==(const vec2& v, const vec2& u);
constexpr bool operator!=(const vec2& v, const vec2& u);

// vector functions
constexpr float length(const vec2& v);
constexpr float length2(const vec2& v);
constexpr float dot(const vec2& v, const vec2& u);
constexpr vec2 normalize(const vec2& v);
constexpr vec2 floor(const vec2& v);
constexpr vec2 step(const vec2& v, const vec2& u);
constexpr vec2 step(float e, const vec2& v);
constexpr vec2 max(const vec2& v, const vec2& u);
constexpr vec2 max(const vec2& v, float y);
constexpr vec2 min(const vec2& v, const vec2& u);
constexpr vec2 min(const vec2& v, float y);
constexpr vec2 abs(const vec2& v);

#endif