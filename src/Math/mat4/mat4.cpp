#include "../Math.hpp"

mat4::operator float*() {
  return m;
}

float& mat4::operator[] (int n) {
  return m[n];
}

mat4& mat4::operator+= (const mat4& n) {
  m[0] += n[0];
  m[1] += n[1];
  m[2] += n[2];
  m[3] += n[3];
  m[4] += n[4];
  m[5] += n[5];
  m[6] += n[6];
  m[7] += n[7];
  m[8] += n[8];
  m[9] += n[9];
  m[10] += n[10];
  m[11] += n[11];
  m[12] += n[12];
  m[13] += n[13];
  m[14] += n[14];
  m[15] += n[15];
  return *this;
}
mat4& mat4::operator+= (float s) {
  m[0] += s;
  m[1] += s;
  m[2] += s;
  m[3] += s;
  m[4] += s;
  m[5] += s;
  m[6] += s;
  m[7] += s;
  m[8] += s;
  m[9] += s;
  m[10] += s;
  m[11] += s;
  m[12] += s;
  m[13] += s;
  m[14] += s;
  m[15] += s;
  return *this;
}

mat4& mat4::operator-= (const mat4& n) {
  m[0] -= n[0];
  m[1] -= n[1];
  m[2] -= n[2];
  m[3] -= n[3];
  m[4] -= n[4];
  m[5] -= n[5];
  m[6] -= n[6];
  m[7] -= n[7];
  m[8] -= n[8];
  m[9] -= n[9];
  m[10] -= n[10];
  m[11] -= n[11];
  m[12] -= n[12];
  m[13] -= n[13];
  m[14] -= n[14];
  m[15] -= n[15];
  return *this;
}
mat4& mat4::operator-= (float s) {
  m[0] -= s;
  m[1] -= s;
  m[2] -= s;
  m[3] -= s;
  m[4] -= s;
  m[5] -= s;
  m[6] -= s;
  m[7] -= s;
  m[8] -= s;
  m[9] -= s;
  m[10] -= s;
  m[11] -= s;
  m[12] -= s;
  m[13] -= s;
  m[14] -= s;
  m[15] -= s;
  return *this;
}

mat4& mat4::operator*= (float s) {
  m[0] *= s;
  m[1] *= s;
  m[2] *= s;
  m[3] *= s;
  m[4] *= s;
  m[5] *= s;
  m[6] *= s;
  m[7] *= s;
  m[8] *= s;
  m[9] *= s;
  m[10] *= s;
  m[11] *= s;
  m[12] *= s;
  m[13] *= s;
  m[14] *= s;
  m[15] *= s;
  return *this;
}
mat4& mat4::operator*= (const mat4& n) {
  *this = *this * n;
  return *this;
}

mat4& mat4::operator/= (float s) {
  m[0] /= s;
  m[1] /= s;
  m[2] /= s;
  m[3] /= s;
  m[4] /= s;
  m[5] /= s;
  m[6] /= s;
  m[7] /= s;
  m[8] /= s;
  m[9] /= s;
  m[10] /= s;
  m[11] /= s;
  m[12] /= s;
  m[13] /= s;
  m[14] /= s;
  m[15] /= s;
  return *this;
}

mat4& mat4::operator= (const mat4& n) {
  m[0] = n[0];
  m[1] = n[1];
  m[2] = n[2];
  m[3] = n[3];
  m[4] = n[4];
  m[5] = n[5];
  m[6] = n[6];
  m[7] = n[7];
  m[8] = n[8];
  m[9] = n[9];
  m[10] = n[10];
  m[11] = n[11];
  m[12] = n[12];
  m[13] = n[13];
  m[14] = n[14];
  m[15] = n[15];
  return *this;
}

mat4& mat4::identity () {
  *this = identity4();
  return *this;
}
mat4& mat4::transpose () {
  *this = ::transpose(*this);
  return *this;
}
mat4& mat4::inverse () {
  *this = ::inverse(*this);
  return *this;
}
