#ifndef MATH_MATHCD_HPP
#define MATH_MATHCD_HPP

#include "Math.hpp"

struct Rect {
  vec2 topleft;
  vec2 size;

  Rect();
  Rect(vec2 tl, vec2 s);
  Rect(vec2 tl, float sx, float sy);
  Rect(float x, float y, float sx, float sy);
  Rect(const Rect& r);

  bool contains(const vec2& point) const;
  vec2 middle() const;
  void middle(const vec2& m);
  vec2 bottomright() const;
  void bottomright(const vec2& br);
};


#endif