#include "LFont.hpp"

#include "../../Resource/Font.hpp"
#include <sol.hpp>

#include "../Math/LMath.hpp"

std::vector<std::pair<std::function<void(sol::state& state)>, std::string>>
  font_dependencies = {
    std::make_pair(&Lua::math_as_lua, "vec2"),
  };

void Lua::font_as_lua(sol::state& state) {
  // load the dependencies of this library
  for (auto a : font_dependencies) {
    auto exists = state[a.second];

    if (!exists.valid())
      a.first(state);
  }

  /* sol::constructors<sol::types<std::string>> ctor; */

  /* sol::usertype<Font> type(ctor); */

  /* state.set_usertype("Font", type); */
}
