#include "LDropdown.hpp"

#include "../../GUI/Dropdown.hpp"
#include "../../Input/Event.hpp"

#include <sol.hpp>

void Lua::dropdown_as_lua(sol::state& state) {

  auto guiTable = state["GUI"];

  if (!guiTable.valid())
    state.create_named_table("GUI");

  sol::table GUITable = state["GUI"];

  sol::constructors<
    sol::types<const std::vector<std::string>&, const mmm::vec2&>> dropdownCtor;

  // clang-format off
  sol::usertype<Dropdown> dropdownType(dropdownCtor,
    // Inherited from GUI
    // Overloading functions
    "isVisible"  , sol::overload((bool(Dropdown::*)() const) &Dropdown::isVisible,
                                 (void(Dropdown::*)(bool)) &Dropdown::isVisible),

    "isClickable", sol::overload((bool(Dropdown::*)() const) &Dropdown::isClickable,
                                 (void(Dropdown::*)(bool)) &Dropdown::isClickable),

    "isAnimating", sol::overload((bool(Dropdown::*)() const) &Dropdown::isAnimating,
                                 (void(Dropdown::*)(bool)) &Dropdown::isAnimating),

    "isMinimized", sol::overload((bool(Dropdown::*)() const) &Dropdown::isMinimized,
                                 (void(Dropdown::*)(bool)) &Dropdown::isMinimized),

    "isMouseOver", sol::overload((bool(Dropdown::*)() const) &Dropdown::isMouseOver,
                                 (void(Dropdown::*)(bool)) &Dropdown::isMouseOver),

    "hasChanged" , sol::overload((bool(Dropdown::*)() const) &Dropdown::hasChanged,
                                 (void(Dropdown::*)(bool)) &Dropdown::hasChanged),

    "update"               , &Dropdown::update,
    "isInside"             , &Dropdown::isInside,
    "input"                , &Dropdown::input,
    "setInputHandler"      , &Dropdown::setInputHandler,
    "defaultInputHandler"  , &Dropdown::defaultInputHandler,
    "box"                  , &Dropdown::box,
    "position"             , &Dropdown::position,
    "size"                 , &Dropdown::size,
    "offset"               , &Dropdown::offset,
    "setSize"              , &Dropdown::setSize,

    // Functions specific to Menu
    "setActiveItem", sol::overload((bool(Dropdown::*)(const mmm::vec2&)) &Dropdown::setActiveItem,
                                   (void(Dropdown::*)(std::string)) &Dropdown::setActiveItem),

    "defaultInputHandler" , &Dropdown::defaultInputHandler,
    "isInsideOptionsList" , &Dropdown::isInsideOptionsList,
    "isInsideDropItem"    , &Dropdown::isInsideDropItem,
    "setMouseOverItem"    , &Dropdown::setMouseOverItem,
    "setActiveItemIndex"  , &Dropdown::setActiveItemIndex,
    "activeItemText"      , &Dropdown::activeItemText,
    "activeItemIndex"     , &Dropdown::activeItemIndex,
    "setPosition"         , &Dropdown::setPosition,
    "setOffset"           , &Dropdown::setOffset,
    "draw"                , &Dropdown::draw,
    sol::base_classes, sol::bases<GUI>());

  GUITable.set_usertype("Dropdown", dropdownType);
  // clang-format on
}
