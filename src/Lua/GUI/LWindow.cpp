#include "LWindow.hpp"

#include "../../GUI/Checkbox.hpp"
#include "../../GUI/Dropdown.hpp"
#include "../../GUI/Inputbox.hpp"
#include "../../GUI/Menu.hpp"
#include "../../GUI/Slider.hpp"
#include "../../GUI/Text.hpp"
#include "../../GUI/Tooltip.hpp"
#include "../../GUI/Window.hpp"
#include "../../Input/Event.hpp"

#include <sol.hpp>

void Lua::window_as_lua(sol::state& state) {

  auto guiTable = state["GUI"];

  if (!guiTable.valid())
    state.create_named_table("GUI");

  sol::table GUITable = state["GUI"];

  sol::constructors<sol::types<>, sol::types<std::string&, const Rectangle&>>
    windowCtor;

  // clang-format off
  sol::usertype<Window> windowType(windowCtor,
    // Inherited from GUI
    // Overloading functions
    "isVisible"  , sol::overload((bool(Window::*)() const) &Window::isVisible,
                                 (void(Window::*)(bool)) &Window::isVisible),

    "isClickable", sol::overload((bool(Window::*)() const) &Window::isClickable,
                                 (void(Window::*)(bool)) &Window::isClickable),

    "isAnimating", sol::overload((bool(Window::*)() const) &Window::isAnimating,
                                 (void(Window::*)(bool)) &Window::isAnimating),

    "isMinimized", sol::overload((bool(Window::*)() const) &Window::isMinimized,
                                 (void(Window::*)(bool)) &Window::isMinimized),

    "isMouseOver", sol::overload((bool(Window::*)() const) &Window::isMouseOver,
                                 (void(Window::*)(bool)) &Window::isMouseOver),

    "hasChanged" , sol::overload((bool(Window::*)() const) &Window::hasChanged,
                                 (void(Window::*)(bool)) &Window::hasChanged),

    "update"             , &Window::update,
    "isInside"           , &Window::isInside,
    "input"              , &Window::input,
    "setInputHandler"    , &Window::setInputHandler,
    "defaultInputHandler", &Window::defaultInputHandler,
    "box"                , &Window::box,
    "position"           , &Window::position,
    "size"               , &Window::size,
    "offset"             , &Window::offset,
    "setSize"            , &Window::setSize,

    // Functions specific to window
    "addTitle"         , &Window::addTitle,
    "add"              , &Window::add,
    "addWindow"        , &Window::addWindow,
    "addMenu"          , &Window::addMenu,
    "addMenuItem"      , &Window::addMenuItem,
    "addSlider"        , &Window::addSlider,
    "addDropdown"      , &Window::addDropdown,
    "addCheckbox"      , &Window::addCheckbox,
    "addInputbox"      , &Window::addInputbox,
    "setActiveMenuItem", &Window::setActiveMenuItem,
    "menu"             , &Window::menu,
    "slider"           , &Window::slider,
    "dropdown"         , &Window::dropdown,
    "checkbox"         , &Window::checkbox,
    "inputbox"         , &Window::inputbox,
    "window"           , &Window::window,
    "menues"           , &Window::menues,
    "sliders"          , &Window::sliders,
    "dropdowns"        , &Window::dropdowns,
    "checkboxes"       , &Window::checkboxes,
    "inputboxes"       , &Window::inputboxes,
    "windows"          , &Window::windows,
    "handleKeyInput"   , &Window::handleKeyInput,
    "handleMouseButton", &Window::handleMouseButton,
    "handleAction"     , &Window::handleAction,
    "setOffset"        , &Window::setOffset,
    "draw"             , &Window::draw,
    sol::base_classes, sol::bases<GUI>());

  GUITable.set_usertype("Window", windowType);
  // clang-format on
}
