#include "Rectangle.hpp"

void Rectangle::bottomright(const mmm::vec2& br) {
  mmm::vec2 s = br - topleft;
  if (s.x < 0) {
    s.x       = mmm::max(topleft.x, br.x) - mmm::min(topleft.x, br.x);
    topleft.x = br.x;
  }
  if (s.y < 0) {
    s.y       = mmm::max(topleft.y, br.y) - mmm::min(topleft.y, br.y);
    topleft.y = br.y;
  }
  size = s;
}
