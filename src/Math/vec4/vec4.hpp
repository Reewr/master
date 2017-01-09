#ifndef VEC4_HPP
#define VEC4_HPP

struct vec4 {

  union {
    float v[4];

    #include "vec4_swizzle.tpp"
  };

           constexpr vec4 ();
  explicit constexpr vec4 (float x);
  explicit constexpr vec4 (const float* u);
           constexpr vec4 (float x, float y, float z, float w);
  explicit constexpr vec4 (const vec2& xy, float z, float w);
  explicit constexpr vec4 (float x, const vec2& yz, float w);
  explicit constexpr vec4 (float x, float y, const vec2& zw);
  explicit constexpr vec4 (const vec2& xy, const vec2 zw);
  explicit constexpr vec4 (const vec3& xyz, float w);
  explicit constexpr vec4 (float x, const vec3& yzw);

  constexpr float operator [] (int n);

         operator float* ();
  float& operator []     (int n);
  vec4&  operator +=     (float s);
  vec4&  operator +=     (const vec4& u);
  vec4&  operator -=     (float s);
  vec4&  operator -=     (const vec4& u);
  vec4&  operator *=     (float s);
  vec4&  operator *=     (const vec4& u);
  vec4&  operator /=     (float s);
  vec4&  operator /=     (const vec4& u);
  vec4&  operator =      (const vec4& u);
  vec4&  normalize       ();
};

constexpr vec4 operator +  (float s, const vec4& v);
constexpr vec4 operator +  (const vec4& v, float s);
constexpr vec4 operator +  (const vec4& v, const vec4& u);

constexpr vec4 operator -  (const vec4& v);
constexpr vec4 operator -  (float s, const vec4& v);
constexpr vec4 operator -  (const vec4& v, float s);
constexpr vec4 operator -  (const vec4& v, const vec4& u);

constexpr vec4 operator *  (float s, const vec4& v);
constexpr vec4 operator *  (const vec4& v, float s);
constexpr vec4 operator *  (const vec4& v, const vec4& u);

constexpr vec4 operator /  (float s, const vec4& v);
constexpr vec4 operator /  (const vec4& v, float s);
constexpr vec4 operator /  (const vec4& v, const vec4& u);

constexpr bool operator == (const vec4& v, const vec4& u);
constexpr bool operator != (const vec4& v, const vec4& u);

// vector functions
constexpr float length    (const vec4& v);
constexpr float length2   (const vec4& v);
constexpr float dot       (const vec4& v, const vec4& u);
constexpr vec4  normalize (const vec4& v);
constexpr vec4  floor     (const vec4& v);
constexpr vec4  step      (const vec4& v, const vec4& u);
constexpr vec4  step      (float e, const vec4& v);
constexpr vec4  max       (const vec4& v, const vec4& u);
constexpr vec4  max       (const vec4& v, float y);
constexpr vec4  min       (const vec4& v, const vec4& u);
constexpr vec4  min       (const vec4& v, float y);
constexpr vec4  abs       (const vec4& v);

#endif