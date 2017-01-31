#include "LText.hpp"

#include "../../GUI/Text.hpp"
#include "../../Input/Event.hpp"
#include "../../Resource/Font.hpp"

#include <sol.hpp>

void Lua::text_as_lua(sol::state& state) {

  auto guiTable = state["GUI"];

  if (!guiTable.valid())
    state.create_named_table("GUI");

  sol::table GUITable = state["GUI"];

  // clang-format off
  GUITable["TextColor"] = GUITable.create_with(
    "Black" , Text::BLACK,
    "White" , Text::WHITE,
    "Redj"  , Text::RED,
    "Green" , Text::GREEN,
    "Blue"  , Text::BLUE,
    "Yellow", Text::YELLOW);

  GUITable["TextStyle"] = GUITable.create_with(
    "Bold"     ,  Text::BOLD,
    "Underline",  Text::UNDERLINE,
    "Italic"   ,  Text::ITALIC);

  sol::constructors<
    sol::types<std::string, std::string, mmm::vec2>,
    sol::types<std::string, std::string, mmm::vec2, int>,
    sol::types<std::string, std::string, mmm::vec2, int, int>,
    sol::types<std::string, std::string, mmm::vec2, int, int, mmm::vec2>> ctor;

  // clang-format off
  sol::usertype<Text> type(ctor,
    // Inherited from GUI
    // Overloading functions
    "isVisible"  , sol::overload((bool(Text::*)() const) &Text::isVisible,
                                 (void(Text::*)(bool)) &Text::isVisible),

    "isClickable", sol::overload((bool(Text::*)() const) &Text::isClickable,
                                 (void(Text::*)(bool)) &Text::isClickable),

    "isAnimating", sol::overload((bool(Text::*)() const) &Text::isAnimating,
                                 (void(Text::*)(bool)) &Text::isAnimating),

    "isMinimized", sol::overload((bool(Text::*)() const) &Text::isMinimized,
                                 (void(Text::*)(bool)) &Text::isMinimized),

    "isMouseOver", sol::overload((bool(Text::*)() const) &Text::isMouseOver,
                                 (void(Text::*)(bool)) &Text::isMouseOver),

    "hasChanged" , sol::overload((bool(Text::*)() const) &Text::hasChanged,
                                 (void(Text::*)(bool)) &Text::hasChanged),

    "update"             , &Text::update,
    "isInside"           , &Text::isInside,
    "input"              , &Text::input,
    "setInputHandler"    , &Text::setInputHandler,
    "defaultInputHandler", &Text::defaultInputHandler,
    "box"                , &Text::box,
    "position"           , &Text::position,
    "size"               , &Text::size,
    "offset"             , &Text::offset,
    "setSize"            , &Text::setSize,
    "setOffset"          , &Text::setOffset,

    // Overriden and custom functions
    "draw"              , &Text::draw,
    "setStyle"          , &Text::setStyle,
    "setLimit"          , &Text::setLimit,
    "setText"           , &Text::setText,
    "setTextSize"       , &Text::setTextSize,
    "setPrevColor"      , &Text::setPrevColor,
    "setPosition"       , &Text::setPosition,
    "hasBackgroundColor", &Text::hasBackgroundColor,
    "getText"           , &Text::getText,
    "getFormattedText"  , &Text::getFormattedText,
    "getCharSize"       , &Text::getCharSize,
    sol::base_classes, sol::bases<GUI>());

  GUITable.set_usertype("Text", type);
  // clang-format on
}
