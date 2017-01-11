#ifndef MATH_MVP_HPP
#define MATH_MVP_HPP

struct MVP {
  constexpr MVP();
  constexpr MVP(mat4 m, mat4 v, mat4 p);
  mat4 model;
  mat4 view;
  mat4 proj;

  constexpr operator mat4();
};

#include "MVP.tpp"

#endif