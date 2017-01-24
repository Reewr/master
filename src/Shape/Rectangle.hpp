#pragma once

#include <mmm.hpp>

struct Rectangle {
  mmm::vec2 topleft;
  mmm::vec2 size;

  constexpr Rectangle();
  constexpr Rectangle(const mmm::vec2& tl, const mmm::vec2& s);
  constexpr Rectangle(const mmm::vec2& tl, float sx, float sy);
  constexpr Rectangle(float x, float y, float sx, float sy);
  constexpr Rectangle(const Rectangle& r);

  constexpr bool contains(const mmm::vec2& point) const;
  constexpr mmm::vec2 middle() const;
  constexpr mmm::vec2 bottomright() const;
  void bottomright(const mmm::vec2& br);
};

// ----------------------------------------------------------
//
// Below follows the implementation of the constexpr functions
//
// ----------------------------------------------------------

constexpr Rectangle::Rectangle() {}

constexpr Rectangle::Rectangle(const mmm::vec2& tl, const mmm::vec2& s)
    : topleft(tl), size(s) {}

constexpr Rectangle::Rectangle(float x, float y, float sx, float sy)
    : topleft(x, y), size(sx, sy) {}

constexpr Rectangle::Rectangle(const mmm::vec2& tl, float sx, float sy)
    : topleft(tl), size(sx, sy) {}

constexpr Rectangle::Rectangle(const Rectangle& r)
    : topleft(r.topleft), size(r.size) {}

constexpr bool Rectangle::contains(const mmm::vec2& p) const {
  return p.x >= topleft.x && p.y >= topleft.y && p.x <= topleft.x + size.x &&
         p.y <= topleft.y + size.y;
}

constexpr mmm::vec2 Rectangle::middle() const {
  return mmm::vec2(topleft.x + size.x / 2, topleft.y + size.y / 2);
}

constexpr mmm::vec2 Rectangle::bottomright() const {
  return mmm::vec2(topleft.x + size.x, topleft.y + size.y);
}