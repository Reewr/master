#ifndef MATH_VEC3_TPP
#define MATH_VEC3_TPP

constexpr vec3::vec3() : v{ 0 } {}
constexpr vec3::vec3(float x) : v{ x, x, x } {}
constexpr vec3::vec3(float x, float y, float z) : v{ x, y, z } {}
constexpr vec3::vec3(const float* u) : v{ u[0], u[1], u[2] } {}
constexpr vec3::vec3(const vec2& u, float z) : v{ u[0], u[1], z } {}
constexpr vec3::vec3(float x, const vec2& u) : v{ x, u[0], u[1] } {}
constexpr vec3::vec3(const vec4& u) : v{ u[0], u[1], u[2] } {}

constexpr float vec3::operator[](int n) {
  return v[n];
}

constexpr vec3 operator+(float s, const vec3& v) {
  return vec3(s + v[0], s + v[1], s + v[2]);
}
constexpr vec3 operator+(const vec3& v, float s) {
  return vec3(v[0] + s, v[1] + s, v[2] + s);
}
constexpr vec3 operator+(const vec3& v, const vec3& u) {
  return vec3(v[0] + u[0], v[1] + u[1], v[2] + u[2]);
}

constexpr vec3 operator-(const vec3& v) {
  return vec3(-v[0], -v[1], -v[2]);
}
constexpr vec3 operator-(float s, const vec3& v) {
  return vec3(s - v[0], s - v[1], s - v[2]);
}
constexpr vec3 operator-(const vec3& v, float s) {
  return vec3(v[0] - s, v[1] - s, v[2] - s);
}
constexpr vec3 operator-(const vec3& v, const vec3& u) {
  return vec3(v[0] - u[0], v[1] - u[1], v[2] - u[2]);
}

constexpr vec3 operator*(float s, const vec3& v) {
  return vec3(s * v[0], s * v[1], s * v[2]);
}
constexpr vec3 operator*(const vec3& v, float s) {
  return vec3(v[0] * s, v[1] * s, v[2] * s);
}
constexpr vec3 operator*(const vec3& v, const vec3& u) {
  return vec3(v[0] * u[0], v[1] * u[1], v[2] * u[2]);
}

constexpr vec3 operator/(float s, const vec3& v) {
  return vec3(s / v[0], s / v[1], s / v[2]);
}
constexpr vec3 operator/(const vec3& v, float s) {
  return vec3(v[0] / s, v[1] / s, v[2] / s);
}
constexpr vec3 operator/(const vec3& v, const vec3& u) {
  return vec3(v[0] / u[0], v[1] / u[1], v[2] / u[2]);
}

constexpr bool operator==(const vec3& v, const vec3& u) {
  return v[0] == u[0] && v[1] == u[1] && v[2] == u[2];
}
constexpr bool operator!=(const vec3& v, const vec3& u) {
  return !(v == u);
}

constexpr float length(const vec3& v) {
  return sqrt(v[0] * v[0] + v[1] * v[1] + v[2] * v[2]);
}
constexpr float length2(const vec3& v) {
  return v[0] * v[0] + v[1] * v[1] + v[2] * v[2];
}
constexpr float dot(const vec3& v, const vec3& u) {
  return v[0] * u[0] + v[1] * u[1] + v[2] * u[2];
}
constexpr vec3 cross(const vec3& v, const vec3& u) {
  return vec3(v[1] * u[2] - v[2] * u[1],
              v[2] * u[0] - v[0] * u[2],
              v[0] * u[1] - v[1] * u[0]);
}
constexpr vec3 normalize(const vec3& v) {
  return v / length(v);
}

constexpr vec3 floor(const vec3& v) {
  return vec3(floor(v[0]), floor(v[1]), floor(v[2]));
}
constexpr vec3 step(const vec3& v, const vec3& u) {
  return vec3(step(v[0], u[0]), step(v[1], u[1]), step(v[2], u[2]));
}
constexpr vec3 step(float e, const vec3& v) {
  return vec3(step(e, v[0]), step(e, v[1]), step(e, v[2]));
}
constexpr vec3 max(const vec3& v, const vec3& u) {
  return vec3(max(v[0], u[0]), max(v[1], u[1]), max(v[2], u[2]));
}
constexpr vec3 max(const vec3& v, float y) {
  return vec3(max(v[0], y), max(v[1], y), max(v[2], y));
}
constexpr vec3 min(const vec3& v, const vec3& u) {
  return vec3(min(v[0], u[0]), min(v[1], u[1]), min(v[2], u[2]));
}
constexpr vec3 min(const vec3& v, float y) {
  return vec3(min(v[0], y), min(v[1], y), min(v[2], y));
}
constexpr vec3 abs(const vec3& v) {
  return vec3(std::abs(v[0]), std::abs(v[1]), std::abs(v[2]));
}


#endif