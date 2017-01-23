#pragma once

#include "../Math/Math.hpp"

struct Rectangle {
  vec2 topleft;
  vec2 size;

  constexpr Rectangle();
  constexpr Rectangle(const vec2& tl, const vec2& s);
  constexpr Rectangle(const vec2& tl, float sx, float sy);
  constexpr Rectangle(float x, float y, float sx, float sy);
  constexpr Rectangle(const Rectangle& r);

  constexpr bool contains(const vec2& point) const;
  constexpr vec2 middle() const;
  constexpr vec2 bottomright() const;
  void bottomright(const vec2& br);
};

// ----------------------------------------------------------
//
// Below follows the implementation of the constexpr functions
//
// ----------------------------------------------------------

constexpr Rectangle::Rectangle() {}

constexpr Rectangle::Rectangle(const vec2& tl, const vec2& s)
    : topleft(tl), size(s) {}

constexpr Rectangle::Rectangle(float x, float y, float sx, float sy)
    : topleft(x, y), size(sx, sy) {}

constexpr Rectangle::Rectangle(const vec2& tl, float sx, float sy)
    : topleft(tl), size(sx, sy) {}

constexpr Rectangle::Rectangle(const Rectangle& r)
    : topleft(r.topleft), size(r.size) {}

constexpr bool Rectangle::contains(const vec2& p) const {
  return p.x >= topleft.x && p.y >= topleft.y && p.x <= topleft.x + size.x &&
         p.y <= topleft.y + size.y;
}

constexpr vec2 Rectangle::middle() const {
  return vec2(topleft.x + size.x / 2, topleft.y + size.y / 2);
}

constexpr vec2 Rectangle::bottomright() const {
  return vec2(topleft.x + size.x, topleft.y + size.y);
}