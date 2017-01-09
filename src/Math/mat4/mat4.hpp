#ifndef MATH_MAT4_HPP
#define MATH_MAT4_HPP

struct mat4 {
  float m[16];

  constexpr mat4 ();

  constexpr mat4 (
    float a, float b, float c, float d,
    float e, float f, float g, float h,
    float i, float j, float k, float l,
    float m, float n, float o, float p
  );

  explicit constexpr mat4 (float a);

  constexpr float operator[] (int n);

  operator float*();
  float& operator[] (int n);
  mat4& operator+= (const mat4& n);
  mat4& operator+= (float s);
  mat4& operator-= (const mat4& n);
  mat4& operator-= (float s);
  mat4& operator*= (const mat4& n);
  mat4& operator*= (float s);
  mat4& operator/= (float s);
  mat4& operator= (const mat4& n);

  mat4& identity ();
  mat4& transpose ();
  mat4& inverse ();
};

constexpr mat4 operator+ (const mat4& m, float s);
constexpr mat4 operator+ (float s, const mat4& m);
constexpr mat4 operator- (const mat4& m, float s);
constexpr mat4 operator- (const mat4& m);
constexpr mat4 operator- (float s, const mat4& m);
constexpr vec4 operator* (const mat4& m, const vec4& v);
constexpr mat4 operator* (const mat4& m, const mat4& n);
constexpr mat4 operator* (const mat4& m, float s);
constexpr mat4 operator* (float s, const mat4& m);
constexpr mat4 operator/ (const mat4& m, float s);
constexpr bool operator== (const mat4& m, const mat4& n);
constexpr bool operator!= (const mat4& m, const mat4& n);

// matrix functions
constexpr float determinant (const mat4& m);
constexpr mat4 transpose (const mat4& m);
constexpr mat4 inverse (const mat4& m);

// affine matrices
// ...that's one fine matrix!
constexpr mat4 identity4 ();
constexpr mat4 translate (float x, float y, float z);
constexpr mat4 translate (const vec3& v);
constexpr mat4 rotate_x (float d);
constexpr mat4 rotate_y (float d);
constexpr mat4 rotate_z (float d);
constexpr mat4 rotate (float x, float y, float z);
constexpr mat4 rotate (const vec3& v);
constexpr mat4 scale (float x, float y, float z);
constexpr mat4 scale (const vec3& v);

#endif