#include "LMenu.hpp"

#include "../../GUI/Menu.hpp"
#include "../../Input/Event.hpp"

#include <sol.hpp>

void Lua::menu_as_lua(sol::state& state) {

  auto guiTable = state["GUI"];

  if (!guiTable.valid())
    state.create_named_table("GUI");

  sol::table GUITable = state["GUI"];

  // Enable the menu settings
  sol::constructors<
    sol::types<float>,
    sol::types<float, float>,
    sol::types<float, float, int>,
    sol::types<float, float, int, int>> menuSettingsCtors;

  sol::usertype<Menu::MenuSettings> menuSettings(menuSettingsCtors,
    "size"  , &Menu::MenuSettings::size,
    "offset", &Menu::MenuSettings::offset,
    "ori"   , &Menu::MenuSettings::ori,
    "color" , &Menu::MenuSettings::color);

  GUITable.create_named("Orientation",
      "Horizontal", Menu::HORIZONTAL,
      "Vertical"  , Menu::VERTICAL);

  GUITable.set_usertype("MenuSettings", menuSettings);

  sol::constructors<
    sol::types<const std::string&, const mmm::vec2&>,
    sol::types<const std::string&,
               const mmm::vec2&,
               const Menu::MenuSettings&>,

    sol::types<const std::vector<std::string>&, const mmm::vec2&>,
    sol::types<const std::vector<std::string>&,
               const mmm::vec2&,
               const Menu::MenuSettings&>> menuCtors;

  // clang-format off
  sol::usertype<Menu> menuType(menuCtors,
    // Inherited from GUI
    // Overloading functions
    "isVisible"  , sol::overload((bool(Menu::*)() const) &Menu::isVisible,
                                 (void(Menu::*)(bool)) &Menu::isVisible),

    "isClickable", sol::overload((bool(Menu::*)() const) &Menu::isClickable,
                                 (void(Menu::*)(bool)) &Menu::isClickable),

    "isAnimating", sol::overload((bool(Menu::*)() const) &Menu::isAnimating,
                                 (void(Menu::*)(bool)) &Menu::isAnimating),

    "isMinimized", sol::overload((bool(Menu::*)() const) &Menu::isMinimized,
                                 (void(Menu::*)(bool)) &Menu::isMinimized),

    "isMouseOver", sol::overload((bool(Menu::*)() const) &Menu::isMouseOver,
                                 (void(Menu::*)(bool)) &Menu::isMouseOver),

    "hasChanged" , sol::overload((bool(Menu::*)() const) &Menu::hasChanged,
                                 (void(Menu::*)(bool)) &Menu::hasChanged),

    "update"               , &Menu::update,
    "isInside"             , &Menu::isInside,
    "input"                , &Menu::input,
    "setInputHandler"      , &Menu::setInputHandler,
    "defaultInputHandler"  , &Menu::defaultInputHandler,
    "box"                  , &Menu::box,
    "position"             , &Menu::position,
    "size"                 , &Menu::size,
    "offset"               , &Menu::offset,
    "setSize"              , &Menu::setSize,
    "setOffset"            , &Menu::setOffset,

    // Functions specific to Menu
    "isInsideMenuElement"  , &Menu::isInsideMenuElement,
    "addMenuItem"          , &Menu::addMenuItem,
    "addMenuItems"         , &Menu::addMenuItems,
    "clearMenuItems"       , &Menu::clearMenuItems,
    "setActiveMenu"        , &Menu::setActiveMenu,
    "setActiveMenuKeyboard", &Menu::setActiveMenuKeyboard,
    "getActiveMenu"        , &Menu::getActiveMenu,
    "getActiveMenuItem"    , &Menu::getActiveMenuItem,
    "setOffset"            , &Menu::setOffset,
    "draw"                 , &Menu::draw,
    sol::base_classes, sol::bases<GUI>());

  GUITable.set_usertype("Menu", menuType);
  // clang-format on
}
