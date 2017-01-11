#ifndef MATH_SWIZZLE_HPP
#define MATH_SWIZZLE_HPP

struct vec2;
struct vec3;
struct vec4;

template <int n, int a>
struct swizzle_1 {
  float v[n];

  constexpr operator float();
  operator float&();
  float* operator&();

  float& operator=(float x);
};

template <int n, int a, int b>
struct swizzle_2 {
  float v[n];

  constexpr operator vec2();

  swizzle_2<n, a, b>& operator+=(const vec2& u);
  swizzle_2<n, a, b>& operator+=(float s);
  swizzle_2<n, a, b>& operator-=(const vec2& u);
  swizzle_2<n, a, b>& operator-=(float s);
  swizzle_2<n, a, b>& operator*=(float s);
  swizzle_2<n, a, b>& operator/=(float s);
  swizzle_2<n, a, b>& operator=(const vec2& u);
  swizzle_2<n, a, b>& normalize();
};

template <int n, int a, int b, int c>
struct swizzle_3 {
  float v[n];

  constexpr operator vec3();

  swizzle_3<n, a, b, c>& operator+=(const vec3& u);
  swizzle_3<n, a, b, c>& operator+=(float s);
  swizzle_3<n, a, b, c>& operator-=(const vec3& u);
  swizzle_3<n, a, b, c>& operator-=(float s);
  swizzle_3<n, a, b, c>& operator*=(float s);
  swizzle_3<n, a, b, c>& operator/=(float s);
  swizzle_3<n, a, b, c>& operator=(const vec3& u);
  swizzle_3<n, a, b, c>& normalize();
};

template <int n, int a, int b, int c, int d>
struct swizzle_4 {
  float v[n];

  constexpr operator vec4();

  swizzle_4<n, a, b, c, d>& operator+=(const vec4& u);
  swizzle_4<n, a, b, c, d>& operator+=(float s);
  swizzle_4<n, a, b, c, d>& operator-=(const vec4& u);
  swizzle_4<n, a, b, c, d>& operator-=(float s);
  swizzle_4<n, a, b, c, d>& operator*=(float s);
  swizzle_4<n, a, b, c, d>& operator/=(float s);
  swizzle_4<n, a, b, c, d>& operator=(const vec4& u);
  swizzle_4<n, a, b, c, d>& normalize();
};

#endif