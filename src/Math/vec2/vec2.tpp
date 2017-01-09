#ifndef MATH_VEC2_TPP
#define MATH_VEC2_TPP

constexpr vec2::vec2 () : v{0} {}
constexpr vec2::vec2 (float x) : v{x,x} {}
constexpr vec2::vec2 (float x, float y) : v{x,y} {}
constexpr vec2::vec2 (const float* u) : v{u[0],u[1]} {}
constexpr vec2::vec2 (const vec3& u) : v{u[0],u[1]} {}
constexpr vec2::vec2 (const vec4& u) : v{u[0],u[1]} {}

constexpr float vec2::operator [] (int n) {
  return v[n];
}

constexpr vec2 operator +  (float s, const vec2& v) {
  return vec2(s+v[0], s+v[1]);
}
constexpr vec2 operator +  (const vec2& v, float s) {
  return vec2(v[0]+s, v[1]+s);
}
constexpr vec2 operator +  (const vec2& v, const vec2& u) {
  return vec2(v[0]+u[0], v[1]+u[1]);
}

constexpr vec2 operator -  (const vec2& v) {
  return vec2(-v[0], -v[1]);
}
constexpr vec2 operator -  (float s, const vec2& v) {
  return vec2(s-v[0], s-v[1]);
}
constexpr vec2 operator -  (const vec2& v, float s) {
  return vec2(v[0]-s, v[1]-s);
}
constexpr vec2 operator -  (const vec2& v, const vec2& u) {
  return vec2 (v[0]-u[0], v[1]-u[1]);
}

constexpr vec2 operator *  (float s, const vec2& v) {
  return vec2(s*v[0], s*v[1]);
}
constexpr vec2 operator *  (const vec2& v, float s) {
  return vec2(v[0]*s, v[1]*s);
}
constexpr vec2 operator *  (const vec2& v, const vec2& u) {
  return vec2 (v[0]*u[0], v[1]*u[1]);
}

constexpr vec2 operator /  (float s, const vec2& v) {
  return vec2(s/v[0], s/v[1]);
}
constexpr vec2 operator /  (const vec2& v, float s) {
  return vec2(v[0]/s, v[1]/s);
}
constexpr vec2 operator /  (const vec2& v, const vec2& u) {
  return vec2 (v[0]/u[0], v[1]/u[1]);
}

constexpr bool operator == (const vec2& v, const vec2& u) {
  return v[0] == u[0] && v[1] == u[1];
}
constexpr bool operator != (const vec2& v, const vec2& u) {
  return !(v == u);
}

constexpr float length    (const vec2& v) {
  return sqrt(v[0]*v[0] + v[1]*v[1]);
}
constexpr float length2   (const vec2& v) {
  return v[0]*v[0] + v[1]*v[1];
}
constexpr float dot       (const vec2& v, const vec2& u) {
  return v[0]*u[0] + v[1]*u[1];
}
constexpr vec2  normalize (const vec2& v) {
  return v / length (v);
}

constexpr vec2 floor(const vec2& v) {
  return vec2(floor(v[0]), floor(v[1]));
}

constexpr vec2 step(const vec2& v, const vec2& u) {
  return vec2(step(v[0], u[0]), step(v[1], u[1]));
}

constexpr vec2 step(float e, const vec2& v) {
  return vec2(step(e, v[0]), step(e, v[1]));
}
constexpr vec2 max(const vec2& v, const vec2& u){
  return vec2(max(v[0], u[0]), max(v[1], u[1]));
}
constexpr vec2 max(const vec2& v, float y){
  return vec2(max(v[0], y), max(v[1], y));
}
constexpr vec2 min(const vec2& v, const vec2& u){
  return vec2(min(v[0], u[0]), min(v[1], u[1]));
}
constexpr vec2 min(const vec2& v, float y){
  return vec2(min(v[0], y), min(v[1], y));
}
constexpr vec2 abs(const vec2& v) {
  return vec2(std::abs(v[0]), std::abs(v[1]));
}

#endif