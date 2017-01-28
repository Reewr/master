#include "LInputbox.hpp"

#include "../../GUI/Inputbox.hpp"
#include "../../Input/Event.hpp"
#include "../../Shape/Rectangle.hpp"

#include <sol.hpp>

void Lua::inputbox_as_lua(sol::state& state) {

  auto guiTable = state["GUI"];

  if (!guiTable.valid())
    state.create_named_table("GUI");

  sol::table GUITable = state["GUI"];

  sol::constructors<
    sol::types<const Rectangle&>,
    sol::types<const Rectangle&, const std::string&>> inputboxCtor;

  // clang-format off
  sol::usertype<Inputbox> dropdownType(inputboxCtor,
    // Inherited from GUI
    // Overloading functions
    "isVisible"  , sol::overload((bool(Inputbox::*)() const) &Inputbox::isVisible,
                                 (void(Inputbox::*)(bool)) &Inputbox::isVisible),

    "isClickable", sol::overload((bool(Inputbox::*)() const) &Inputbox::isClickable,
                                 (void(Inputbox::*)(bool)) &Inputbox::isClickable),

    "isAnimating", sol::overload((bool(Inputbox::*)() const) &Inputbox::isAnimating,
                                 (void(Inputbox::*)(bool)) &Inputbox::isAnimating),

    "isMinimized", sol::overload((bool(Inputbox::*)() const) &Inputbox::isMinimized,
                                 (void(Inputbox::*)(bool)) &Inputbox::isMinimized),

    "isMouseOver", sol::overload((bool(Inputbox::*)() const) &Inputbox::isMouseOver,
                                 (void(Inputbox::*)(bool)) &Inputbox::isMouseOver),

    "hasChanged" , sol::overload((bool(Inputbox::*)() const) &Inputbox::hasChanged,
                                 (void(Inputbox::*)(bool)) &Inputbox::hasChanged),

    "update"               , &Inputbox::update,
    "isInside"             , &Inputbox::isInside,
    "input"                , &Inputbox::input,
    "setInputHandler"      , &Inputbox::setInputHandler,
    "defaultInputHandler"  , &Inputbox::defaultInputHandler,
    "box"                  , &Inputbox::box,
    "position"             , &Inputbox::position,
    "size"                 , &Inputbox::size,
    "offset"               , &Inputbox::offset,
    "setSize"              , &Inputbox::setSize,


    // Functions specific to Menu
    "showInputbox"        , &Inputbox::showInputbox,
    "text"                , &Inputbox::text,
    "changeText"          , &Inputbox::changeText,
    "defaultInputHandler" , &Inputbox::defaultInputHandler,
    "setOffset"           , &Inputbox::setOffset,
    "draw"                , &Inputbox::draw,
    sol::base_classes, sol::bases<GUI>());

  GUITable.set_usertype("Inputbox", dropdownType);
  // clang-format on
}
