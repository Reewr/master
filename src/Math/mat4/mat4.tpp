#ifndef MATH_MAT4_TPP
#define MATH_MAT4_TPP

constexpr mat4::mat4() : m{ 0 } {}
constexpr mat4::mat4(float a,
                     float b,
                     float c,
                     float d,
                     float e,
                     float f,
                     float g,
                     float h,
                     float i,
                     float j,
                     float k,
                     float l,
                     float m,
                     float n,
                     float o,
                     float p)
    : m{ a, b, c, d, e, f, g, h, i, j, k, l, m, n, o, p } {}

constexpr mat4::mat4(float a)
    : m{ a, a, a, a, a, a, a, a, a, a, a, a, a, a, a, a } {}

constexpr float mat4::operator[](int n) {
  return m[n];
}

constexpr mat4 operator+(const mat4& m, float s) {
  return mat4(m[0] + s,
              m[1] + s,
              m[2] + s,
              m[3] + s,
              m[4] + s,
              m[5] + s,
              m[6] + s,
              m[7] + s,
              m[8] + s,
              m[9] + s,
              m[10] + s,
              m[11] + s,
              m[12] + s,
              m[13] + s,
              m[14] + s,
              m[15] + s);
}
constexpr mat4 operator+(float s, const mat4& m) {
  return mat4(s + m[0],
              s + m[1],
              s + m[2],
              s + m[3],
              s + m[4],
              s + m[5],
              s + m[6],
              s + m[7],
              s + m[8],
              s + m[9],
              s + m[10],
              s + m[11],
              s + m[12],
              s + m[13],
              s + m[14],
              s + m[15]);
}

constexpr mat4 operator-(const mat4& m, float s) {
  return mat4(m[0] - s,
              m[1] - s,
              m[2] - s,
              m[3] - s,
              m[4] - s,
              m[5] - s,
              m[6] - s,
              m[7] - s,
              m[8] - s,
              m[9] - s,
              m[10] - s,
              m[11] - s,
              m[12] - s,
              m[13] - s,
              m[14] - s,
              m[15] - s);
}
constexpr mat4 operator-(const mat4& m) {
  return mat4(-m[0],
              -m[1],
              -m[2],
              -m[3],
              -m[4],
              -m[5],
              -m[6],
              -m[7],
              -m[8],
              -m[9],
              -m[10],
              -m[11],
              -m[12],
              -m[13],
              -m[14],
              -m[15]);
}
constexpr mat4 operator-(float s, const mat4& m) {
  return mat4(s - m[0],
              s - m[1],
              s - m[2],
              s - m[3],
              s - m[4],
              s - m[5],
              s - m[6],
              s - m[7],
              s - m[8],
              s - m[9],
              s - m[10],
              s - m[11],
              s - m[12],
              s - m[13],
              s - m[14],
              s - m[15]);
}

constexpr vec4 operator*(const mat4& m, const vec4& v) {
  return vec4(m[0] * v[0] + m[1] * v[1] + m[2] * v[2] + m[3] * v[3],
              m[4] * v[0] + m[5] * v[1] + m[6] * v[2] + m[7] * v[3],
              m[8] * v[0] + m[9] * v[1] + m[10] * v[2] + m[11] * v[3],
              m[12] * v[0] + m[13] * v[1] + m[14] * v[2] + m[15] * v[3]);
}
constexpr mat4 operator*(const mat4& m, const mat4& n) {
  return mat4(m[0] * n[0] + m[1] * n[4] + m[2] * n[8] + m[3] * n[12],
              m[0] * n[1] + m[1] * n[5] + m[2] * n[9] + m[3] * n[13],
              m[0] * n[2] + m[1] * n[6] + m[2] * n[10] + m[3] * n[14],
              m[0] * n[3] + m[1] * n[7] + m[2] * n[11] + m[3] * n[15],

              m[4] * n[0] + m[5] * n[4] + m[6] * n[8] + m[7] * n[12],
              m[4] * n[1] + m[5] * n[5] + m[6] * n[9] + m[7] * n[13],
              m[4] * n[2] + m[5] * n[6] + m[6] * n[10] + m[7] * n[14],
              m[4] * n[3] + m[5] * n[7] + m[6] * n[11] + m[7] * n[15],

              m[8] * n[0] + m[9] * n[4] + m[10] * n[8] + m[11] * n[12],
              m[8] * n[1] + m[9] * n[5] + m[10] * n[9] + m[11] * n[13],
              m[8] * n[2] + m[9] * n[6] + m[10] * n[10] + m[11] * n[14],
              m[8] * n[3] + m[9] * n[7] + m[10] * n[11] + m[11] * n[15],

              m[12] * n[0] + m[13] * n[4] + m[14] * n[8] + m[15] * n[12],
              m[12] * n[1] + m[13] * n[5] + m[14] * n[9] + m[15] * n[13],
              m[12] * n[2] + m[13] * n[6] + m[14] * n[10] + m[15] * n[14],
              m[12] * n[3] + m[13] * n[7] + m[14] * n[11] + m[15] * n[15]);
}
constexpr mat4 operator*(const mat4& m, float s) {
  return mat4(m[0] * s,
              m[1] * s,
              m[2] * s,
              m[3] * s,
              m[4] * s,
              m[5] * s,
              m[6] * s,
              m[7] * s,
              m[8] * s,
              m[9] * s,
              m[10] * s,
              m[11] * s,
              m[12] * s,
              m[13] * s,
              m[14] * s,
              m[15] * s);
}
constexpr mat4 operator*(float s, const mat4& m) {
  return mat4(s * m[0],
              s * m[1],
              s * m[2],
              s * m[3],
              s * m[4],
              s * m[5],
              s * m[6],
              s * m[7],
              s * m[8],
              s * m[9],
              s * m[10],
              s * m[11],
              s * m[12],
              s * m[13],
              s * m[14],
              s * m[15]);
}

constexpr mat4 operator/(const mat4& m, float s) {
  return mat4(m[0] / s,
              m[1] / s,
              m[2] / s,
              m[3] / s,
              m[4] / s,
              m[5] / s,
              m[6] / s,
              m[7] / s,
              m[8] / s,
              m[9] / s,
              m[10] / s,
              m[11] / s,
              m[12] / s,
              m[13] / s,
              m[14] / s,
              m[15] / s);
}

constexpr bool operator==(const mat4& m, const mat4& n) {
  return m[0] == n[0] && m[1] == n[1] && m[2] == n[2] && m[3] == n[3] &&
         m[4] == n[4] && m[5] == n[5] && m[6] == n[6] && m[7] == n[7] &&
         m[8] == n[8] && m[9] == n[9] && m[10] == n[10] && m[11] == n[11] &&
         m[12] == n[12] && m[13] == n[13] && m[14] == n[14] && m[15] == n[15];
}
constexpr bool operator!=(const mat4& m, const mat4& n) {
  return !(m == n);
}

constexpr float determinant(const mat4& m) {
  return m[0] * (m[5] * (m[10] * m[15] - m[11] * m[14]) -
                 m[6] * (m[9] * m[15] - m[11] * m[13]) +
                 m[7] * (m[9] * m[14] - m[10] * m[13])) -
         m[1] * (m[4] * (m[10] * m[15] - m[11] * m[14]) -
                 m[6] * (m[8] * m[15] - m[11] * m[12]) +
                 m[7] * (m[8] * m[14] - m[10] * m[12])) +
         m[2] * (m[4] * (m[9] * m[15] - m[11] * m[13]) -
                 m[5] * (m[8] * m[15] - m[11] * m[12]) +
                 m[7] * (m[8] * m[13] - m[9] * m[12])) -
         m[3] * (m[4] * (m[9] * m[14] - m[10] * m[13]) -
                 m[5] * (m[8] * m[14] - m[10] * m[12]) +
                 m[6] * (m[8] * m[13] - m[9] * m[12]));
}
constexpr mat4 transpose(const mat4& m) {
  return mat4(m[0],
              m[4],
              m[8],
              m[12],
              m[1],
              m[5],
              m[9],
              m[13],
              m[2],
              m[6],
              m[10],
              m[14],
              m[4],
              m[7],
              m[11],
              m[15]);
}
constexpr mat4 inverse(const mat4& m) {
  return 1 / determinant(m) * mat4(m[5] * (m[10] * m[15] - m[11] * m[14]) -
                                     m[6] * (m[9] * m[15] - m[11] * m[13]) +
                                     m[7] * (m[9] * m[14] - m[10] * m[13]),
                                   -m[1] * (m[10] * m[15] - m[11] * m[14]) +
                                     m[2] * (m[9] * m[15] - m[11] * m[13]) -
                                     m[3] * (m[9] * m[14] - m[10] * m[13]),
                                   m[1] * (m[6] * m[15] - m[7] * m[14]) -
                                     m[2] * (m[5] * m[15] - m[7] * m[13]) +
                                     m[3] * (m[5] * m[14] - m[6] * m[13]),
                                   -m[1] * (m[6] * m[11] - m[7] * m[10]) +
                                     m[2] * (m[5] * m[11] - m[7] * m[9]) -
                                     m[3] * (m[5] * m[10] - m[6] * m[9]),

                                   -m[4] * (m[10] * m[15] - m[11] * m[14]) +
                                     m[6] * (m[8] * m[15] - m[11] * m[12]) -
                                     m[7] * (m[8] * m[14] - m[10] * m[12]),
                                   m[0] * (m[10] * m[15] - m[11] * m[14]) -
                                     m[2] * (m[8] * m[15] - m[11] * m[12]) +
                                     m[3] * (m[8] * m[14] - m[10] * m[12]),
                                   -m[0] * (m[6] * m[15] - m[7] * m[14]) +
                                     m[2] * (m[4] * m[15] - m[7] * m[12]) -
                                     m[3] * (m[4] * m[14] - m[6] * m[12]),
                                   m[0] * (m[6] * m[11] - m[7] * m[10]) -
                                     m[2] * (m[4] * m[11] - m[7] * m[8]) +
                                     m[3] * (m[4] * m[10] - m[6] * m[8]),

                                   m[4] * (m[9] * m[15] - m[11] * m[13]) -
                                     m[5] * (m[8] * m[15] - m[11] * m[12]) +
                                     m[7] * (m[8] * m[13] - m[9] * m[12]),
                                   -m[0] * (m[9] * m[15] - m[11] * m[13]) +
                                     m[1] * (m[8] * m[15] - m[11] * m[12]) -
                                     m[3] * (m[8] * m[13] - m[9] * m[12]),
                                   m[0] * (m[5] * m[15] - m[7] * m[13]) -
                                     m[1] * (m[4] * m[15] - m[7] * m[12]) +
                                     m[3] * (m[4] * m[13] - m[5] * m[12]),
                                   -m[0] * (m[5] * m[11] - m[7] * m[9]) +
                                     m[1] * (m[4] * m[11] - m[7] * m[8]) -
                                     m[3] * (m[4] * m[9] - m[5] * m[8]),

                                   -m[4] * (m[9] * m[14] - m[10] * m[13]) +
                                     m[5] * (m[8] * m[14] - m[10] * m[12]) -
                                     m[6] * (m[8] * m[13] - m[9] * m[12]),
                                   m[0] * (m[9] * m[14] - m[10] * m[13]) -
                                     m[1] * (m[8] * m[14] - m[10] * m[12]) +
                                     m[2] * (m[8] * m[13] - m[9] * m[12]),
                                   -m[0] * (m[5] * m[14] - m[6] * m[13]) +
                                     m[1] * (m[4] * m[14] - m[6] * m[12]) -
                                     m[2] * (m[4] * m[13] - m[5] * m[12]),
                                   m[0] * (m[5] * m[10] - m[6] * m[9]) -
                                     m[1] * (m[4] * m[10] - m[6] * m[8]) +
                                     m[2] * (m[4] * m[9] - m[5] * m[8]));
}

constexpr mat4 identity4() {
  return mat4(1, 0, 0, 0, 0, 1, 0, 0, 0, 0, 1, 0, 0, 0, 0, 1);
}
constexpr mat4 translate(float x, float y, float z) {
  return mat4(1, 0, 0, x, 0, 1, 0, y, 0, 0, 1, z, 0, 0, 0, 1);
}
constexpr mat4 translate(const vec3& v) {
  return mat4(1, 0, 0, v[0], 0, 1, 0, v[1], 0, 0, 1, v[2], 0, 0, 0, 1);
}
constexpr mat4 rotate_x(float d) {
  return mat4(1,
              0,
              0,
              0,
              0,
              cos(d * PI / 180.0),
              -sin(d * PI / 180.0),
              0,
              0,
              sin(d * PI / 180.0),
              cos(d * PI / 180.0),
              0,
              0,
              0,
              0,
              1);
}
constexpr mat4 rotate_y(float d) {
  return mat4(cos(d * PI / 180.0),
              0,
              sin(d * PI / 180.0),
              0,
              0,
              1,
              0,
              0,
              -sin(d * PI / 180.0),
              0,
              cos(d * PI / 180.0),
              0,
              0,
              0,
              0,
              1);
}
constexpr mat4 rotate_z(float d) {
  return mat4(cos(d * PI / 180.0),
              -sin(d * PI / 180.0),
              0,
              0,
              sin(d * PI / 180.0),
              cos(d * PI / 180.0),
              0,
              0,
              0,
              0,
              1,
              0,
              0,
              0,
              0,
              1);
}
constexpr mat4 rotate(float x, float y, float z) {
  return rotate_z(z) * rotate_y(y) * rotate_x(x);
}
constexpr mat4 rotate(const vec3& v) {
  return rotate_z(v.z) * rotate_y(v.y) * rotate_x(v.x);
}
constexpr mat4 scale(float x, float y, float z) {
  return mat4(x, 0, 0, 0, 0, y, 0, 0, 0, 0, z, 0, 0, 0, 0, 1);
}
constexpr mat4 scale(const vec3& v) {
  return mat4(v[0], 0, 0, 0, 0, v[1], 0, 0, 0, 0, v[2], 0, 0, 0, 0, 1);
}

#endif