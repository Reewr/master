#include "../Math.hpp"

std::ostream& operator<<(std::ostream& os, const vec2& v) {
  os << "[" << v[0] << ", " << v[1] << "]";
  return os;
}
std::ostream& operator<<(std::ostream& os, const vec3& v) {
  os << "[" << v[0] << ", " << v[1] << ", " << v[2] << "]";
  return os;
}
std::ostream& operator<<(std::ostream& os, const vec4& v) {
  os << "[" << v[0] << ", " << v[1] << ", " << v[2] << ", " << v[3] << "]";
  return os;
}
std::ostream& operator<<(std::ostream& os, const mat3& m) {
  os << std::endl;
  os << "[" << m[0] << ", " << m[1] << ", " << m[2] << "]" << std::endl;
  os << "[" << m[3] << ", " << m[4] << ", " << m[5] << "]" << std::endl;
  os << "[" << m[6] << ", " << m[7] << ", " << m[8] << "]" << std::endl;
  return os;
}
std::ostream& operator<<(std::ostream& os, const mat4& m) {
  os << std::endl;
  os << "[" << m[0] << ", " << m[1] << ", " << m[2] << ", " << m[3] << "]"
     << std::endl;
  os << "[" << m[4] << ", " << m[5] << ", " << m[6] << ", " << m[7] << "]"
     << std::endl;
  os << "[" << m[8] << ", " << m[9] << ", " << m[10] << ", " << m[11] << "]"
     << std::endl;
  os << "[" << m[12] << ", " << m[13] << ", " << m[14] << ", " << m[15] << "]"
     << std::endl;
  return os;
}

void print(const vec2& v) {
  std::cout << "[" << v[0] << ", " << v[1] << "]" << std::endl;
}
void print(const vec3& v) {
  std::cout << "[" << v[0] << ", " << v[1] << ", " << v[2] << "]" << std::endl;
}
void print(const vec4& v) {
  std::cout << "[" << v[0] << ", " << v[1] << ", " << v[2] << ", " << v[3]
            << "]" << std::endl;
}
void print(const mat3& m) {
  std::cout << "[" << m[0] << ", " << m[1] << ", " << m[2] << "]" << std::endl;
  std::cout << "[" << m[3] << ", " << m[4] << ", " << m[5] << "]" << std::endl;
  std::cout << "[" << m[6] << ", " << m[7] << ", " << m[8] << "]" << std::endl;
}
void print(const mat4& m) {
  std::cout << "[" << m[0] << ", " << m[1] << ", " << m[2] << ", " << m[3]
            << "]" << std::endl;
  std::cout << "[" << m[4] << ", " << m[5] << ", " << m[6] << ", " << m[7]
            << "]" << std::endl;
  std::cout << "[" << m[8] << ", " << m[9] << ", " << m[10] << ", " << m[11]
            << "]" << std::endl;
  std::cout << "[" << m[12] << ", " << m[13] << ", " << m[14] << ", " << m[15]
            << "]" << std::endl;
}
