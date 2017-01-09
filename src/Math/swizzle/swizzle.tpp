#ifndef MATH_SWIZZLE_TPP
#define MATH_SWIZZLE_TPP

// casts
  template <int n, int a>
  constexpr swizzle_1<n,a>::operator float () {
    return v[a];
  }
  template <int n, int a>
  swizzle_1<n,a>::operator float& () {
    return v[a];
  }
  template <int n, int a>
  float* swizzle_1<n,a>::operator& () {
    return &v[a];
  }

  template <int n, int a, int b>
  constexpr swizzle_2<n,a,b>::operator vec2 () {
    return vec2(v[a], v[b]);
  }

  template <int n, int a, int b, int c>
  constexpr swizzle_3<n,a,b,c>::operator vec3 () {
    return vec3(v[a], v[b], v[c]);
  }

  template <int n, int a, int b, int c, int d>
  constexpr swizzle_4<n,a,b,c,d>::operator vec4 () {
    return vec4(v[a], v[b], v[c], v[d]);
  }

// operator +=
  template <int n, int a, int b>
  swizzle_2<n,a,b>& swizzle_2<n,a,b>::operator+= (const vec2& u) {
    v[a] += u[0];
    v[b] += u[1];
    return *this;
  }
  template <int n, int a, int b>
  swizzle_2<n,a,b>& swizzle_2<n,a,b>::operator+= (float s) {
    v[a] += s;
    v[b] += s;
    return *this;
  }

  template <int n, int a, int b, int c>
  swizzle_3<n,a,b,c>& swizzle_3<n,a,b,c>::operator+= (const vec3& u) {
    v[a] += u[0];
    v[b] += u[1];
    v[c] += u[2];
    return *this;
  }
  template <int n, int a, int b, int c>
  swizzle_3<n,a,b,c>& swizzle_3<n,a,b,c>::operator+= (float s) {
    v[a] += s;
    v[b] += s;
    v[c] += s;
    return *this;
  }

  template <int n, int a, int b, int c, int d>
  swizzle_4<n,a,b,c,d>& swizzle_4<n,a,b,c,d>::operator+= (const vec4& u) {
    v[a] += u[0];
    v[b] += u[1];
    v[c] += u[2];
    v[d] += u[3];
    return *this;
  }
  template <int n, int a, int b, int c, int d>
  swizzle_4<n,a,b,c,d>& swizzle_4<n,a,b,c,d>::operator+= (float s) {
    v[a] += s;
    v[b] += s;
    v[c] += s;
    v[d] += s;
    return *this;
  }

// operator -=
  template <int n, int a, int b>
  swizzle_2<n,a,b>& swizzle_2<n,a,b>::operator-= (const vec2& u) {
    v[a] -= u[0];
    v[b] -= u[1];
    return *this;
  }
  template <int n, int a, int b>
  swizzle_2<n,a,b>& swizzle_2<n,a,b>::operator-= (float s) {
    v[a] -= s;
    v[b] -= s;
    return *this;
  }

  template <int n, int a, int b, int c>
  swizzle_3<n,a,b,c>& swizzle_3<n,a,b,c>::operator-= (const vec3& u) {
    v[a] -= u[0];
    v[b] -= u[1];
    v[c] -= u[2];
    return *this;
  }
  template <int n, int a, int b, int c>
  swizzle_3<n,a,b,c>& swizzle_3<n,a,b,c>::operator-= (float s) {
    v[a] -= s;
    v[b] -= s;
    v[c] -= s;
    return *this;
  }

  template <int n, int a, int b, int c, int d>
  swizzle_4<n,a,b,c,d>& swizzle_4<n,a,b,c,d>::operator-= (const vec4& u) {
    v[a] -= u[0];
    v[b] -= u[1];
    v[c] -= u[2];
    v[d] -= u[3];
    return *this;
  }
  template <int n, int a, int b, int c, int d>
  swizzle_4<n,a,b,c,d>& swizzle_4<n,a,b,c,d>::operator-= (float s) {
    v[a] -= s;
    v[b] -= s;
    v[c] -= s;
    v[d] -= s;
    return *this;
  }

// operator *=
  template <int n, int a, int b>
  swizzle_2<n,a,b>& swizzle_2<n,a,b>::operator*= (float s) {
    v[a] *= s;
    v[b] *= s;
    return *this;
  }
  template <int n, int a, int b, int c>
  swizzle_3<n,a,b,c>& swizzle_3<n,a,b,c>::operator*= (float s) {
    v[a] *= s;
    v[b] *= s;
    v[c] *= s;
    return *this;
  }
  template <int n, int a, int b, int c, int d>
  swizzle_4<n,a,b,c,d>& swizzle_4<n,a,b,c,d>::operator*= (float s) {
    v[a] *= s;
    v[b] *= s;
    v[c] *= s;
    v[d] *= s;
    return *this;
  }

// operator /=
  template <int n, int a, int b>
  swizzle_2<n,a,b>& swizzle_2<n,a,b>::operator/= (float s) {
    v[a] /= s;
    v[b] /= s;
    return *this;
  }
  template <int n, int a, int b, int c>
  swizzle_3<n,a,b,c>& swizzle_3<n,a,b,c>::operator/= (float s) {
    v[a] /= s;
    v[b] /= s;
    v[c] /= s;
    return *this;
  }
  template <int n, int a, int b, int c, int d>
  swizzle_4<n,a,b,c,d>& swizzle_4<n,a,b,c,d>::operator/= (float s) {
    v[a] /= s;
    v[b] /= s;
    v[c] /= s;
    v[d] /= s;
    return *this;
  }

// operator =
  template <int n, int a>
  float& swizzle_1<n,a>::operator= (float x) {
    return v[a] = x;
  }

  template <int n, int a, int b>
  swizzle_2<n,a,b>& swizzle_2<n,a,b>::operator= (const vec2& u) {
    v[a] = u[0];
    v[b] = u[1];
    return *this;
  }

  template <int n, int a, int b, int c>
  swizzle_3<n,a,b,c>& swizzle_3<n,a,b,c>::operator= (const vec3& u) {
    v[a] = u[0];
    v[b] = u[1];
    v[c] = u[2];
    return *this;
  }

  template <int n, int a, int b, int c, int d>
  swizzle_4<n,a,b,c,d>& swizzle_4<n,a,b,c,d>::operator= (const vec4& u) {
    v[a] = u[0];
    v[b] = u[1];
    v[c] = u[2];
    v[d] = u[3];
    return *this;
  }

// vector functions
  template <int n, int a, int b>
  swizzle_2<n,a,b>& swizzle_2<n,a,b>::normalize () {
    float l = length (*this);
    v[a] /= l;
    v[b] /= l;
    return *this;
  }

  template <int n, int a, int b, int c>
  swizzle_3<n,a,b,c>& swizzle_3<n,a,b,c>::normalize () {
    float l = length (*this);
    v[a] /= l;
    v[b] /= l;
    v[c] /= l;
    return *this;
  }

  template <int n, int a, int b, int c, int d>
  swizzle_4<n,a,b,c,d>& swizzle_4<n,a,b,c,d>::normalize () {
    float l = length (*this);
    v[a] /= l;
    v[b] /= l;
    v[c] /= l;
    v[d] /= l;
    return *this;
  }

#endif
