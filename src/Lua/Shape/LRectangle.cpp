#include "LRectangle.hpp"

#include "../../Shape/Rectangle.hpp"

#include <sol.hpp>

void Lua::rectangle_as_lua(sol::state& state) {

  // clang-format off
  sol::constructors<
    sol::types<>,
    sol::types<const mmm::vec2&, const mmm::vec2&>,
    sol::types<const mmm::vec2&, float, float>,
    sol::types<float, float, float, float>,
    sol::types<const Rectangle&>> ctor;

  sol::usertype<Rectangle> type(ctor,
    "size"       , &Rectangle::size,
    "topleft"    , &Rectangle::topleft,
    "contains"   , &Rectangle::contains,
    "middle"     , &Rectangle::middle,
    "bottomright", sol::overload(
      (mmm::vec2(Rectangle::*)() const)           &Rectangle::bottomright,
      (void     (Rectangle::*)(const mmm::vec2&)) &Rectangle::bottomright));

  state.set_usertype("Rectangle", type);
  // clang-format on
}
