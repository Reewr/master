#pragma once

#include "../Math/Math.hpp"

struct Rectangle {
  vec2 topleft;
  vec2 size;

  Rectangle();
  Rectangle(vec2 tl, vec2 s);
  Rectangle(vec2 tl, float sx, float sy);
  Rectangle(float x, float y, float sx, float sy);
  Rectangle(const Rectangle& r);

  bool contains(const vec2& point) const;
  vec2 middle() const;
  void middle(const vec2& m);
  vec2 bottomright() const;
  void bottomright(const vec2& br);
};