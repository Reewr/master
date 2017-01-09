#ifndef MATH_VEC3_HPP
#define MATH_VEC3_HPP

struct vec3 {

  union {
    float v[3];

    #include "vec3_swizzle.tpp"
  };

           constexpr vec3 ();
  explicit constexpr vec3 (float x);
  explicit constexpr vec3 (const float* u);
           constexpr vec3 (float x, float y, float z);
  explicit constexpr vec3 (float x, const vec2& u);
  explicit constexpr vec3 (const vec2& u, float z);
  explicit constexpr vec3 (const vec4& u);

  constexpr float operator [] (int n);

         operator float* ();
  float& operator []     (int n);
  vec3&  operator +=     (float s);
  vec3&  operator +=     (const vec3& u);
  vec3&  operator -=     (float s);
  vec3&  operator -=     (const vec3& u);
  vec3&  operator *=     (float s);
  vec3&  operator *=     (const vec3& u);
  vec3&  operator /=     (float s);
  vec3&  operator /=     (const vec3& u);
  vec3&  operator =      (const vec3& u);
  vec3&  normalize       ();
};

constexpr vec3 operator +  (float s, const vec3& v);
constexpr vec3 operator +  (const vec3& v, float s);
constexpr vec3 operator +  (const vec3& v, const vec3& u);

constexpr vec3 operator -  (const vec3& v);
constexpr vec3 operator -  (float s, const vec3& v);
constexpr vec3 operator -  (const vec3& v, float s);
constexpr vec3 operator -  (const vec3& v, const vec3& u);

constexpr vec3 operator *  (const vec3& v, float s);
constexpr vec3 operator *  (float s, const vec3& v);
constexpr vec3 operator *  (const vec3& v, const vec3& u);

constexpr vec3 operator /  (const vec3& v, float s);
constexpr vec3 operator /  (float s, const vec3& v);
constexpr vec3 operator /  (const vec3& v, const vec3& u);

constexpr bool operator == (const vec3& v, const vec3& u);
constexpr bool operator != (const vec3& v, const vec3& u);

// vector functions
constexpr float length    (const vec3& v);
constexpr float length2   (const vec3& v);
constexpr float dot       (const vec3& v, const vec3& u);
constexpr vec3  cross     (const vec3& v, const vec3& u);
constexpr vec3  normalize (const vec3& v);
constexpr vec3  floor     (const vec3& v);
constexpr vec3  step      (const vec3& v, const vec3& u);
constexpr vec3  step      (float e, const vec3& v);
constexpr vec3  max       (const vec3& v, const vec3& u);
constexpr vec3  max       (const vec3& v, float y);
constexpr vec3  min       (const vec3& v, const vec3& u);
constexpr vec3  min       (const vec3& v, float y);
constexpr vec3  abs       (const vec3& v);

#endif