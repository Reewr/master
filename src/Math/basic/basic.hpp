#ifndef MATH_BASIC_HPP
#define MATH_BASIC_HPP

constexpr float PI = 3.141592653589793f;

constexpr float radians(float deg);
constexpr float degrees(float rad);

constexpr float sign(float x);
constexpr float mod(float x, float y);
constexpr float min(float x, float y);
constexpr float max(float x, float y);
constexpr float clamp(float x, float low, float hi);
constexpr float mix(float x, float y, float a);
constexpr float step(float e, float x);

#include "basic.tpp"

#endif