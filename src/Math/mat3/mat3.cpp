#include "../Math.hpp"

mat3::operator float*() {
  return m;
}

float& mat3::operator[] (int n) {
  return m[n];
}

mat3& mat3::operator+= (const mat3& n) {
  m[0] += n[0];
  m[1] += n[1];
  m[2] += n[2];
  m[3] += n[3];
  m[4] += n[4];
  m[5] += n[5];
  m[6] += n[6];
  m[7] += n[7];
  m[8] += n[8];
  return *this;
}
mat3& mat3::operator+= (float s) {
  m[0] += s;
  m[1] += s;
  m[2] += s;
  m[3] += s;
  m[4] += s;
  m[5] += s;
  m[6] += s;
  m[7] += s;
  m[8] += s;
  return *this;
}

mat3& mat3::operator-= (const mat3& n) {
  m[0] -= n[0];
  m[1] -= n[1];
  m[2] -= n[2];
  m[3] -= n[3];
  m[4] -= n[4];
  m[5] -= n[5];
  m[6] -= n[6];
  m[7] -= n[7];
  m[8] -= n[8];
  return *this;
}
mat3& mat3::operator-= (float s) {
  m[0] -= s;
  m[1] -= s;
  m[2] -= s;
  m[3] -= s;
  m[4] -= s;
  m[5] -= s;
  m[6] -= s;
  m[7] -= s;
  m[8] -= s;
  return *this;
}

mat3& mat3::operator*= (float s) {
  m[0] *= s;
  m[1] *= s;
  m[2] *= s;
  m[3] *= s;
  m[4] *= s;
  m[5] *= s;
  m[6] *= s;
  m[7] *= s;
  m[8] *= s;
  return *this;
}

mat3& mat3::operator/= (float s) {
  m[0] /= s;
  m[1] /= s;
  m[2] /= s;
  m[3] /= s;
  m[4] /= s;
  m[5] /= s;
  m[6] /= s;
  m[7] /= s;
  m[8] /= s;
  return *this;
}

mat3& mat3::operator= (const mat3& n) {
  m[0] = n[0];
  m[1] = n[1];
  m[2] = n[2];
  m[3] = n[3];
  m[4] = n[4];
  m[5] = n[5];
  m[6] = n[6];
  m[7] = n[7];
  m[8] = n[8];
  return *this;
}

mat3& mat3::identity () {
  *this = identity3();
  return *this;
}
mat3& mat3::transpose () {
  *this = ::transpose(*this);
  return *this;
}
mat3& mat3::inverse () {
  *this = ::inverse(*this);
  return *this;
}
