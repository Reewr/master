#ifndef MATH_MAT3_TPP
#define MATH_MAT3_TPP

constexpr mat3::mat3() : m{ 0 } {}
constexpr mat3::mat3(float a,
                     float b,
                     float c,
                     float d,
                     float e,
                     float f,
                     float g,
                     float h,
                     float i)
    : m{ a, b, c, d, e, f, g, h, i } {}

constexpr float mat3::operator[](int n) {
  return m[n];
}

constexpr mat3 operator+(const mat3& m, float s) {
  return mat3(m[0] + s,
              m[1] + s,
              m[2] + s,
              m[3] + s,
              m[4] + s,
              m[5] + s,
              m[6] + s,
              m[7] + s,
              m[8] + s);
}
constexpr mat3 operator+(float s, const mat3& m) {
  return mat3(s + m[0],
              s + m[1],
              s + m[2],
              s + m[3],
              s + m[4],
              s + m[5],
              s + m[6],
              s + m[7],
              s + m[8]);
}

constexpr mat3 operator-(const mat3& m, float s) {
  return mat3(m[0] - s,
              m[1] - s,
              m[2] - s,
              m[3] - s,
              m[4] - s,
              m[5] - s,
              m[6] - s,
              m[7] - s,
              m[8] - s);
}
constexpr mat3 operator-(const mat3& m) {
  return mat3(-m[0], -m[1], -m[2], -m[3], -m[4], -m[5], -m[6], -m[7], -m[8]);
}
constexpr mat3 operator-(float s, const mat3& m) {
  return mat3(s - m[0],
              s - m[1],
              s - m[2],
              s - m[3],
              s - m[4],
              s - m[5],
              s - m[6],
              s - m[7],
              s - m[8]);
}

constexpr vec3 operator*(const mat3& m, const vec3& v) {
  return vec3(m[0] * v[0] + m[1] * v[1] + m[2] * v[2],
              m[3] * v[0] + m[4] * v[1] + m[5] * v[2],
              m[6] * v[0] + m[7] * v[1] + m[8] * v[2]);
}
constexpr mat3 operator*(const mat3& m, const mat3& n) {
  return mat3(m[0] * n[0] + m[1] * n[3] + m[2] * n[6],
              m[0] * n[1] + m[1] * n[4] + m[2] * n[7],
              m[0] * n[2] + m[1] * n[5] + m[2] * n[8],

              m[3] * n[0] + m[4] * n[3] + m[5] * n[6],
              m[3] * n[1] + m[4] * n[4] + m[5] * n[7],
              m[3] * n[2] + m[4] * n[5] + m[5] * n[8],

              m[6] * n[0] + m[7] * n[3] + m[8] * n[6],
              m[6] * n[1] + m[7] * n[4] + m[8] * n[7],
              m[6] * n[2] + m[7] * n[5] + m[8] * n[8]);
}
constexpr mat3 operator*(const mat3& m, float s) {
  return mat3(m[0] * s,
              m[1] * s,
              m[2] * s,
              m[3] * s,
              m[4] * s,
              m[5] * s,
              m[6] * s,
              m[7] * s,
              m[8] * s);
}
constexpr mat3 operator*(float s, const mat3& m) {
  return mat3(s * m[0],
              s * m[1],
              s * m[2],
              s * m[3],
              s * m[4],
              s * m[5],
              s * m[6],
              s * m[7],
              s * m[8]);
}

constexpr mat3 operator/(const mat3& m, float s) {
  return mat3(m[0] / s,
              m[1] / s,
              m[2] / s,
              m[3] / s,
              m[4] / s,
              m[5] / s,
              m[6] / s,
              m[7] / s,
              m[8] / s);
}

constexpr bool operator==(const mat3& m, const mat3& n) {
  return m[0] == n[0] && m[1] == n[1] && m[2] == n[2] && m[3] == n[3] &&
         m[4] == n[4] && m[5] == n[5] && m[6] == n[6] && m[7] == n[7] &&
         m[8] == n[8];
}
constexpr bool operator!=(const mat3& m, const mat3& n) {
  return !(m == n);
}

constexpr float determinant(const mat3& m) {
  return m[0] * (m[4] * m[8] - m[5] * m[7]) -
         m[1] * (m[3] * m[8] - m[5] * m[6]) +
         m[2] * (m[3] * m[7] - m[4] * m[6]);
}
constexpr mat3 transpose(const mat3& m) {
  return mat3(m[0], m[3], m[6], m[1], m[4], m[7], m[2], m[5], m[8]);
}
constexpr mat3 inverse(const mat3& m) {
  return 1 / determinant(m) * mat3(m[4] * m[8] - m[5] * m[7],
                                   m[2] * m[7] - m[1] * m[8],
                                   m[1] * m[5] - m[2] * m[4],

                                   m[5] * m[6] - m[3] * m[8],
                                   m[0] * m[8] - m[2] * m[6],
                                   m[2] * m[3] - m[0] * m[5],

                                   m[3] * m[7] - m[4] * m[6],
                                   m[1] * m[6] - m[0] * m[7],
                                   m[0] * m[4] - m[1] * m[3]);
}

constexpr mat3 identity3() {
  return mat3(1, 0, 0, 0, 1, 0, 0, 0, 1);
}

#endif