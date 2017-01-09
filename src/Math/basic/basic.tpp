#ifndef MATH_BASIC_TPP
#define MATH_BASIC_TPP

constexpr float radians (float deg) {return deg * PI / 180.0f;}
constexpr float degrees (float rad) {return rad * 180.0f / PI;}

constexpr float sign  (float x) {return x > 0 ? 1 : x < 0 ? -1 : 0;}
constexpr float mod   (float x, float y) {return x - y * floor (x / y);}
constexpr float max   (float x, float y) {return x >= y ? x : y;}
constexpr float min   (float x, float y) {return x <= y ? x : y;}
constexpr float clamp (float x, float l, float h) {return x < l ? l : x > h ? h : x;}
constexpr float mix   (float x, float y, float a) {return x * (1.0f - a) + y * a;}
constexpr float step  (float e, float x) {return x < e ? 0.0 : 1.0;}

#endif