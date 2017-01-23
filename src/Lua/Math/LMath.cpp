#include "LMath.hpp"

#include "../../Math/Math.hpp"
#include <sol.hpp>

void Lua::math_as_lua(sol::state& state) {

  auto v = state["vec2"];

  if (v.valid())
    return;

  sol::constructors<sol::types<int, int>, sol::types<float, float>> ctor;

  sol::usertype<vec2> type(ctor, "x", &vec2::x, "y", &vec2::y);

  state.set_usertype("vec2", type);
}
