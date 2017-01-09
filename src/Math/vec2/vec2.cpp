#include "../Math.hpp"

vec2::operator float* () {
  return v;
}

float& vec2::operator [] (int n) {
  return v[n];
}

vec2&  vec2::operator += (float s) {
  v[0] += s;
  v[1] += s;
  return *this;
}
vec2&  vec2::operator += (const vec2& u) {
  v[0] += u[0];
  v[1] += u[1];
  return *this;
}

vec2&  vec2::operator -= (float s) {
  v[0] -= s;
  v[1] -= s;
  return *this;
}
vec2&  vec2::operator -= (const vec2& u) {
  v[0] -= u[0];
  v[1] -= u[1];
  return *this;
}

vec2&  vec2::operator *= (float s) {
  v[0] *= s;
  v[1] *= s;
  return *this;
}
vec2&  vec2::operator *= (const vec2& u) {
  v[0] *= u[0];
  v[1] *= u[1];
  return *this;
}

vec2&  vec2::operator /= (float s) {
  v[0] /= s;
  v[1] /= s;
  return *this;
}
vec2&  vec2::operator /= (const vec2& u) {
  v[0]  /= u[0];
  v[1]  /= u[1];
  return *this;
}

vec2&  vec2::operator =  (const vec2& u) {
  v[0] = u[0];
  v[1] = u[1];
  return *this;
}

vec2&  vec2::normalize   () {
  float l = length (*this);
  v[0] /= l;
  v[1] /= l;
  return *this;
}
