#include "Rectangle.hpp"

void Rectangle::bottomright(const vec2& br) {
  vec2 s = br - topleft;
  if (s.x < 0) {
    s.x       = max(topleft.x, br.x) - min(topleft.x, br.x);
    topleft.x = br.x;
  }
  if (s.y < 0) {
    s.y       = max(topleft.y, br.y) - min(topleft.y, br.y);
    topleft.y = br.y;
  }
  size = s;
}
