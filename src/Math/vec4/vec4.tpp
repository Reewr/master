#ifndef VEC4_TPP
#define VEC4_TPP

constexpr vec4::vec4() : v{ 0 } {}
constexpr vec4::vec4(float x) : v{ x, x, x, x } {}
constexpr vec4::vec4(const float* u) : v{ u[0], u[1], u[2], u[3] } {}
constexpr vec4::vec4(float x, float y, float z, float w) : v{ x, y, z, w } {}
constexpr vec4::vec4(const vec2& u, float z, float w) : v{ u[0], u[1], z, w } {}
constexpr vec4::vec4(float x, const vec2& u, float w) : v{ x, u[0], u[1], w } {}
constexpr vec4::vec4(float x, float y, const vec2& u) : v{ x, y, u[0], u[1] } {}
constexpr vec4::vec4(const vec2& u, const vec2 t)
    : v{ u[0], u[1], t[0], t[1] } {}
constexpr vec4::vec4(const vec3& u, float w) : v{ u[0], u[1], u[2], w } {}
constexpr vec4::vec4(float x, const vec3& u) : v{ x, u[0], u[1], u[2] } {}

constexpr float vec4::operator[](int n) {
  return v[n];
}

constexpr vec4 operator+(float s, const vec4& v) {
  return vec4(s + v[0], s + v[1], s + v[2], s + v[3]);
}
constexpr vec4 operator+(const vec4& v, float s) {
  return vec4(v[0] + s, v[1] + s, v[2] + s, v[3] + s);
}
constexpr vec4 operator+(const vec4& v, const vec4& u) {
  return vec4(v[0] + u[0], v[1] + u[1], v[2] + u[2], v[3] + u[3]);
}

constexpr vec4 operator-(const vec4& v) {
  return vec4(-v[0], -v[1], -v[2], -v[3]);
}
constexpr vec4 operator-(float s, const vec4& v) {
  return vec4(s - v[0], s - v[1], s - v[2], s - v[3]);
}
constexpr vec4 operator-(const vec4& v, float s) {
  return vec4(v[0] - s, v[1] - s, v[2] - s, v[3] - s);
}
constexpr vec4 operator-(const vec4& v, const vec4& u) {
  return vec4(v[0] - u[0], v[1] - u[1], v[2] - u[2], v[3] - u[3]);
}

constexpr vec4 operator*(float s, const vec4& v) {
  return vec4(s * v[0], s * v[1], s * v[2], s * v[3]);
}
constexpr vec4 operator*(const vec4& v, float s) {
  return vec4(v[0] * s, v[1] * s, v[2] * s, v[3] * s);
}
constexpr vec4 operator*(const vec4& v, const vec4& u) {
  return vec4(v[0] * u[0], v[1] * u[1], v[2] * u[2], v[3] * u[3]);
}

constexpr vec4 operator/(float s, const vec4& v) {
  return vec4(s / v[0], s / v[1], s / v[2], s / v[3]);
}
constexpr vec4 operator/(const vec4& v, float s) {
  return vec4(v[0] / s, v[1] / s, v[2] / s, v[3] / s);
}
constexpr vec4 operator/(const vec4& v, const vec4& u) {
  return vec4(v[0] / u[0], v[1] / u[1], v[2] / u[2], v[3] / u[3]);
}

constexpr bool operator==(const vec4& v, const vec4& u) {
  return v[0] == u[0] && v[1] == u[1] && v[2] == u[2] && v[3] == u[3];
}
constexpr bool operator!=(const vec4& v, const vec4& u) {
  return v[0] != u[0] && v[1] != u[1] && v[2] != u[2] && v[3] != u[3];
}

constexpr float length(const vec4& v) {
  return sqrt(v[0] * v[0] + v[1] * v[1] + v[2] * v[2] + v[3] * v[3]);
}
constexpr float length2(const vec4& v) {
  return v[0] * v[0] + v[1] * v[1] + v[2] * v[2] + v[3] * v[3];
}
constexpr float dot(const vec4& v, const vec4& u) {
  return v[0] * u[0] + v[1] * u[1] + v[2] * u[2] + v[3] * u[3];
}
constexpr vec4 normalize(const vec4& v) {
  return v / length(v);
}

constexpr vec4 floor(const vec4& v) {
  return vec4(floor(v[0]), floor(v[1]), floor(v[2]), floor(v[3]));
}
constexpr vec4 step(const vec4& v, const vec4& u) {
  return vec4(step(v[0], u[0]),
              step(v[1], u[1]),
              step(v[2], u[2]),
              step(v[3], u[3]));
}
constexpr vec4 step(float e, const vec4& v) {
  return vec4(step(e, v[0]), step(e, v[1]), step(e, v[2]), step(e, v[3]));
}
constexpr vec4 max(const vec4& v, const vec4& u) {
  return vec4(max(v[0], u[0]),
              max(v[1], u[1]),
              max(v[2], u[2]),
              max(v[3], u[3]));
}
constexpr vec4 max(const vec4& v, float y) {
  return vec4(max(v[0], y), max(v[1], y), max(v[2], y), max(v[3], y));
}
constexpr vec4 min(const vec4& v, const vec4& u) {
  return vec4(min(v[0], u[0]),
              min(v[1], u[1]),
              min(v[2], u[2]),
              min(v[3], u[3]));
}
constexpr vec4 min(const vec4& v, float y) {
  return vec4(min(v[0], y), min(v[1], y), min(v[2], y), min(v[3], y));
}
constexpr vec4 abs(const vec4& v) {
  return vec4(std::abs(v[0]), std::abs(v[1]), std::abs(v[2]), std::abs(v[3]));
}

#endif