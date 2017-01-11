#ifndef MATH_MVP_TPP
#define MATH_MVP_TPP

constexpr MVP::MVP()
    : model(identity4()), view(identity4()), proj(identity4()) {}
constexpr MVP::MVP(mat4 m, mat4 v, mat4 p) : model(m), view(v), proj(p) {}

constexpr MVP::operator mat4() {
  return proj * view * model;
}

#endif
