#include "../Math.hpp"

vec4::operator float* () {
  return v;
}

float& vec4::operator [] (int n) {
  return v[n];
}

vec4& vec4::operator += (float s) {
  v[0] += s;
  v[1] += s;
  v[2] += s;
  v[3] += s;
  return *this;
}
vec4& vec4::operator += (const vec4& u) {
  v[0] += u[0];
  v[1] += u[1];
  v[2] += u[2];
  v[3] += u[3];
  return *this;
}

vec4& vec4::operator -= (float s) {
  v[0] -= s;
  v[1] -= s;
  v[2] -= s;
  v[3] -= s;
  return *this;
}
vec4& vec4::operator -= (const vec4& u) {
  v[0] -= u[0];
  v[1] -= u[1];
  v[2] -= u[2];
  v[3] -= u[3];
  return *this;
}

vec4& vec4::operator *= (float s) {
  v[0] *= s;
  v[1] *= s;
  v[2] *= s;
  v[3] *= s;
  return *this;
}
vec4& vec4::operator *= (const vec4& u) {
  v[0] *= u[0];
  v[1] *= u[1];
  v[2] *= u[2];
  v[3] *= u[3];
  return *this;
}

vec4& vec4::operator /= (float s) {
  v[0] /= s;
  v[1] /= s;
  v[2] /= s;
  v[3] /= s;
  return *this;
}
vec4& vec4::operator /= (const vec4& u) {
  v[0] /= u[0];
  v[1] /= u[1];
  v[2] /= u[2];
  v[3] /= u[3];
  return *this;
}

vec4& vec4::operator =  (const vec4& u) {
  v[0] = u[0];
  v[1] = u[1];
  v[2] = u[2];
  v[3] = u[3];
  return *this;
}

vec4& vec4::normalize   () {
  float l = length (*this);
  v[0] /= l;
  v[1] /= l;
  v[2] /= l;
  v[3] /= l;
  return *this;
}
