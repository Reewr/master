#include "Text.hpp"

#include "../Resource/Font.hpp"
#include "../GUI/Text.hpp"
#include <sol.hpp>

#include "Font.hpp"
#include "Math.hpp"

std::vector<std::pair<std::function<void(sol::state& state)>, std::string>>
  text_dependencies = { std::make_pair(&Lua::math_as_lua, "vec2") };

void Lua::text_as_lua(sol::state& state) {

  // load the dependencies of this library
  for (auto a : text_dependencies) {
    auto exists = state[a.second];

    if (!exists.valid())
      a.first(state);
  }

  // clang-format off
  state["TextColor"] = state.create_table_with(
    "Black" , Text::BLACK,
    "White" , Text::WHITE,
    "Redj"  , Text::RED,
    "Green" , Text::GREEN,
    "Blue"  , Text::BLUE,
    "Yellow", Text::YELLOW);

  state["TextStyle"] = state.create_table_with(
    "Bold"     ,  Text::BOLD,
    "Underline",  Text::UNDERLINE,
    "Italic"   ,  Text::ITALIC);

  sol::constructors<
    sol::types<std::string, std::string, vec2>,
    sol::types<std::string, std::string, vec2, int>,
    sol::types<std::string, std::string, vec2, int, int>,
    sol::types<std::string, std::string, vec2, int, int, vec2>> ctor;

  sol::usertype<Text> type(ctor,
      "draw"        , &Text::draw,
      "setStyle"    , &Text::setStyle,
      "setLimit"    , &Text::setLimit,
      "setText"     , &Text::setText,
      "setTextSize" , &Text::setTextSize,
      "setColor"    , &Text::setColor,
      "setPrevColor", &Text::setPrevColor,
      "setPosition" , &Text::setPosition,
      "getText"     , &Text::getText,
      "getCharSize" , &Text::getCharSize);

  state.set_usertype("Text", type);
  // clang-format on
}
