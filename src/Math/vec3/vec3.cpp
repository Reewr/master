#include "../Math.hpp"

vec3::operator float*() {
  return v;
}

float& vec3::operator[](int n) {
  return v[n];
}

vec3& vec3::operator+=(float s) {
  v[0] += s;
  v[1] += s;
  v[2] += s;
  return *this;
}
vec3& vec3::operator+=(const vec3& u) {
  v[0] += u[0];
  v[1] += u[1];
  v[2] += u[2];
  return *this;
}

vec3& vec3::operator-=(float s) {
  v[0] -= s;
  v[1] -= s;
  v[2] -= s;
  return *this;
}
vec3& vec3::operator-=(const vec3& u) {
  v[0] -= u[0];
  v[1] -= u[1];
  v[2] -= u[2];
  return *this;
}

vec3& vec3::operator*=(float s) {
  v[0] *= s;
  v[1] *= s;
  v[2] *= s;
  return *this;
}
vec3& vec3::operator*=(const vec3& u) {
  v[0] *= u[0];
  v[1] *= u[1];
  v[2] *= u[2];
  return *this;
}

vec3& vec3::operator/=(float s) {
  v[0] /= s;
  v[1] /= s;
  v[2] /= s;
  return *this;
}
vec3& vec3::operator/=(const vec3& u) {
  v[0] /= u[0];
  v[1] /= u[1];
  v[2] /= u[2];
  return *this;
}

vec3& vec3::operator=(const vec3& u) {
  v[0] = u[0];
  v[1] = u[1];
  v[2] = u[2];
  return *this;
}

vec3& vec3::normalize() {
  float l = length(*this);
  v[0] /= l;
  v[1] /= l;
  v[2] /= l;
  return *this;
}
