#include "MathCD.hpp"

#include <cmath>

Rect::Rect() {}

Rect::Rect(vec2 tl, vec2 s) : topleft(tl), size(s) {}

Rect::Rect(float x, float y, float sx, float sy) {
  topleft = vec2(x, y);
  size    = vec2(sx, sy);
}

Rect::Rect(vec2 tl, float sx, float sy) {
  topleft = tl;
  size    = vec2(sx, sy);
}

Rect::Rect(const Rect& r) {
  topleft = r.topleft;
  size    = r.size;
}

bool Rect::contains(const vec2& p) const {
  return p.x >= topleft.x && p.y >= topleft.y && p.x <= topleft.x + size.x &&
         p.y <= topleft.y + size.y;
}

vec2 Rect::middle() const {
  return vec2(topleft.x + size.x / 2, topleft.y + size.y / 2);
}

void Rect::middle(const vec2& m) {
  bottomright(m * 2);
}

vec2 Rect::bottomright() const {
  return vec2(topleft.x + size.x, topleft.y + size.y);
}

void Rect::bottomright(const vec2& br) {
  vec2 s = br - topleft;
  if (s.x < 0) {
    s.x       = fmax(topleft.x, br.x) - fmin(topleft.x, br.x);
    topleft.x = br.x;
  }
  if (s.y < 0) {
    s.y       = fmax(topleft.y, br.y) - fmin(topleft.y, br.y);
    topleft.y = br.y;
  }
  size = s;
}
