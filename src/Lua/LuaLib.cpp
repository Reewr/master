#include "LuaLib.hpp"

#include <mmm.hpp>
#include <sol.hpp>

#include "../3D/Spider.hpp"
#include "../Console/Console.hpp"
#include "../Drawable/Drawable3D.hpp"
#include "../GUI/Checkbox.hpp"
#include "../GUI/Dropdown.hpp"
#include "../GUI/Inputbox.hpp"
#include "../GUI/Menu.hpp"
#include "../GUI/Slider.hpp"
#include "../GUI/Text.hpp"
#include "../GUI/Window.hpp"
#include "../Input/Event.hpp"
#include "../Input/Input.hpp"
#include "../State/State.hpp"
#include "../Utils/CFG.hpp"

#include "../Learning/SpiderSwarm.hpp"
#include <Parameters.h>
#include <Substrate.h>

/**
 * @brief
 *   Exposes the console and its available functions to Lua.
 *   Since there should never be another instance of Console,
 *   the `new` function is set to be a no_constructor.
 *
 * @param state
 *
 * @return
 */
sol::table LuaLib::openConsole(sol::this_state state) {
  sol::state_view lua(state);
  sol::table      module = lua.create_table();

  // clang-format off
  module.new_usertype<Console>("_Console",
    "new"    , sol::no_constructor,
    "log"    , &Console::log,
    "error"  , &Console::error,
    "warn"   , &Console::warn,
    "clear"  , &Console::clear);
  // clang-format on

  return module;
}

/**
 * @brief
 *   Exposes the CFG and all its sub-classes that are used.
 *   Since the children arent suppose to be used without the
 *   main CFG class, they've been hidden by using `_` in front
 *   of their names.
 *
 * @param state
 *
 * @return
 */
sol::table LuaLib::openCFG(sol::this_state state) {
  sol::state_view lua(state);
  sol::table      module = lua.create_table();

  // Add Graphics as a lua struct, so it can
  // be accessed when using CFG.
  //
  // Scoping is done to make this section slightly
  // bit cleaner
  //
  // CFG::Graphics
  // clang-format off
  {
    // Add the type of constructors, since we don't have one
    // we just say no types.
    sol::constructors<sol::types<>> ctor;

    // add the variables that should be accessible via lua.
    sol::usertype<CFG::Graphics> type(ctor,
      "aliasing"     , &CFG::Graphics::aliasing,
      "aniso"        , &CFG::Graphics::aniso,
      "aspect"       , &CFG::Graphics::aspect,
      "monitor"      , &CFG::Graphics::monitor,
      "res"          , &CFG::Graphics::res,
      "shadowSamples", &CFG::Graphics::shadowSamples,
      "shadowRes"    , &CFG::Graphics::shadowRes,
      "vsync"        , &CFG::Graphics::vsync,
      "viewDistance" , &CFG::Graphics::viewDistance,
      "winMode"      , &CFG::Graphics::winMode);

    module.set_usertype("_CFG_Graphics", type);
  }

  // CFG::General
  {
    sol::constructors<sol::types<>> ctor;

    // add the variables that should be accessible via lua.
    sol::usertype<CFG::General> type(ctor,
      "debug", &CFG::General::debug,
      "debugColors", &CFG::General::debugColors);

    module.set_usertype("_CFG_General", type);
  }

  // CFG::Camera
  {
    sol::constructors<sol::types<>> ctor;

    // add the variables that should be accessible via lua.
    sol::usertype<CFG::Camera> type(ctor,
      "rotSpeed" , &CFG::Camera::rotSpeed,
      "rotInvH"  , &CFG::Camera::rotInvH,
      "rotInvV"  , &CFG::Camera::rotInvV,
      "zoomSpeed", &CFG::Camera::zoomSpeed,
      "zoomInv"  , &CFG::Camera::zoomInv);

    module.set_usertype("_CFG_Camera", type);
  }

  // CFG::Console
  {
    sol::constructors<sol::types<>> ctor;

    sol::usertype<CFG::Console> type(ctor,
      "enabled", &CFG::Console::enabled);

    module.set_usertype("_CFG_Console", type);
  }

  // ActB
  {
    sol::constructors<sol::types<>, sol::types<int>, sol::types<int, int>> ctor;

    sol::usertype<ActB> type(ctor,
        "key1", &ActB::key1,
        "key2", &ActB::key2);

    module.set_usertype("_CFG_ActB", type);
  }

  // CFG::Bindings
  {

    sol::constructors<sol::types<>> ctor;

    // add the variables that should be accessible via lua.
    sol::usertype<CFG::Bindings> type(ctor,
      "moveUp"     , &CFG::Bindings::moveUp,
      "moveDown"   , &CFG::Bindings::moveDown,
      "moveLeft"   , &CFG::Bindings::moveLeft,
      "moveRight"  , &CFG::Bindings::moveRight,
      "pauseMenu"  , &CFG::Bindings::pauseMenu,
      "rotate"     , &CFG::Bindings::rotate,
      "showConsole", &CFG::Bindings::showConsole,
      "screenshot" , &CFG::Bindings::screenshot);

    module.set_usertype("_CFG_Bindings", type);
  }

  // CFG::Audio
  {
    sol::constructors<sol::types<>> ctor;

    // add the variables that should be accessible via lua.
    sol::usertype<CFG::Audio> type(ctor,
      "SFXVolume"   , &CFG::Audio::SFXVolume,
      "musicVolume" , &CFG::Audio::musicVolume,
      "masterVolume", &CFG::Audio::masterVolume);

    module.set_usertype("_CFG_Audio", type);
  }

  // Finally, since all the child structures of the CFG has been
  // defined, we can now define the CFG itself.
  sol::constructors<sol::types<>> ctor;

  // add the variables that should be accessible via lua.
  sol::usertype<CFG> type(ctor,
    "graphics", &CFG::graphics,
    "general" , &CFG::general,
    "camera"  , &CFG::camera,
    "console" , &CFG::console,
    "bindings", &CFG::bindings,
    "audio"   , &CFG::audio);

  module.set_usertype("CFG", type);
  // clang-format on

  // At this point, you can load the state and use the CFG
  // as such:
  //
  // myCFG = CFG.new()
  // print(myCFG.graphics.res.x)
  // print(myCFG.bindings.moveUp.key1)
  // -- and so on.
  return module["CFG"];
}

/**
 * @brief
 *   Creates a new usertype in Lua that exposes the
 *   Dropdown class. It creates a module that so
 *   that it is not exposed globally.
 *
 * @param state
 *   When a Lua file requires the file, Lua will call
 *   this function (however only once) to return
 *   the module.
 *
 * @return
 */
sol::table LuaLib::GUI::openDropdown(sol::this_state state) {
  sol::state_view lua(state);

  sol::table module = lua.create_table();

  sol::constructors<
    sol::types<const std::vector<std::string>&, const mmm::vec2&>>
    dropdownCtor;

  // clang-format off
  sol::usertype<Dropdown> dropdownType(dropdownCtor,
    // Inherited from GUI
    // Overloading functions
    "isVisible", sol::overload(
                   (bool(Dropdown::*)() const) &Dropdown::isVisible,
                   (void(Dropdown::*)(bool))   &Dropdown::isVisible),

    "isClickable", sol::overload(
                     (bool(Dropdown::*)() const) &Dropdown::isClickable,
                     (void(Dropdown::*)(bool))   &Dropdown::isClickable),

    "isAnimating", sol::overload(
                     (bool(Dropdown::*)() const) &Dropdown::isAnimating,
                     (void(Dropdown::*)(bool))   &Dropdown::isAnimating),

    "isMinimized", sol::overload(
                     (bool(Dropdown::*)() const) &Dropdown::isMinimized,
                     (void(Dropdown::*)(bool))   &Dropdown::isMinimized),

    "isMouseOver", sol::overload(
                     (bool(Dropdown::*)() const) &Dropdown::isMouseOver,
                     (void(Dropdown::*)(bool))   &Dropdown::isMouseOver),

    "hasChanged", sol::overload(
                    (bool(Dropdown::*)() const) &Dropdown::hasChanged,
                    (void(Dropdown::*)(bool))   &Dropdown::hasChanged),

    "update"             , &Dropdown::update,
    "isInside"           , &Dropdown::isInside,
    "input"              , &Dropdown::input,
    "setInputHandler"    , &Dropdown::setInputHandler,
    "defaultInputHandler", &Dropdown::defaultInputHandler,
    "box"                , &Dropdown::box,
    "position"           , &Dropdown::position,
    "size"               , &Dropdown::size,
    "offset"             , &Dropdown::offset,
    "setSize"            , &Dropdown::setSize,

    // Functions specific to Menu
    "setActiveItem",
      sol::overload(
          (bool(Dropdown::*)(const mmm::vec2&)) &Dropdown::setActiveItem,
          (void(Dropdown::*)(std::string))      &Dropdown::setActiveItem),

    "defaultInputHandler", &Dropdown::defaultInputHandler,
    "isInsideOptionsList", &Dropdown::isInsideOptionsList,
    "isInsideDropItem"   , &Dropdown::isInsideDropItem,
    "setMouseOverItem"   , &Dropdown::setMouseOverItem,
    "setActiveItemIndex" , &Dropdown::setActiveItemIndex,
    "activeItemText"     , &Dropdown::activeItemText,
    "activeItemIndex"    , &Dropdown::activeItemIndex,
    "setPosition"        , &Dropdown::setPosition,
    "setOffset"          , &Dropdown::setOffset,
    "draw"               , &Dropdown::draw,
    sol::base_classes, sol::bases<class GUI>());

  module.set_usertype("Dropdown", dropdownType);
  // clang-format on

  return module["Dropdown"];
}

/**
 * @brief
 *   Exposes the Inputbox class to Lua as a module
 *
 * @param state
 *
 * @return
 */
sol::table LuaLib::GUI::openInputbox(sol::this_state state) {
  sol::state_view lua(state);

  sol::table module = lua.create_table();

  sol::constructors<sol::types<const Rectangle&>,
                    sol::types<const Rectangle&, const std::string&>>
    inputboxCtor;

  // clang-format off
  sol::usertype<Inputbox> dropdownType(inputboxCtor,
    // Inherited from GUI
    // Overloading functions
    "isVisible", sol::overload(
                   (bool(Inputbox::*)() const) &Inputbox::isVisible,
                   (void(Inputbox::*)(bool))   &Inputbox::isVisible),

    "isClickable", sol::overload(
                     (bool(Inputbox::*)() const) &Inputbox::isClickable,
                     (void(Inputbox::*)(bool))   &Inputbox::isClickable),

    "isAnimating", sol::overload(
                     (bool(Inputbox::*)() const) &Inputbox::isAnimating,
                     (void(Inputbox::*)(bool))   &Inputbox::isAnimating),

    "isMinimized", sol::overload(
                     (bool(Inputbox::*)() const) &Inputbox::isMinimized,
                     (void(Inputbox::*)(bool))   &Inputbox::isMinimized),

    "isMouseOver", sol::overload(
                     (bool(Inputbox::*)() const) &Inputbox::isMouseOver,
                     (void(Inputbox::*)(bool))   &Inputbox::isMouseOver),

    "hasChanged", sol::overload(
                    (bool(Inputbox::*)() const) &Inputbox::hasChanged,
                    (void(Inputbox::*)(bool))   &Inputbox::hasChanged),

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
    sol::base_classes, sol::bases<class GUI>());

  module.set_usertype("Inputbox", dropdownType);
  // clang-format on

  return module["Inputbox"];
}

/**
 * @brief
 *   Exposes the Menu class to Lua as a module together
 *   with all its enum methods
 *
 * @param state
 *
 * @return
 */
sol::table LuaLib::GUI::openMenu(sol::this_state state) {
  sol::state_view lua(state);
  sol::table      module = lua.create_table();

  // Enable the menu settings
  // clang-format off

  sol::constructors<
    sol::types<const std::string&, const mmm::vec2&>,
    sol::types<const std::string&,
               const mmm::vec2&,
               const Menu::MenuSettings&>,

    sol::types<const std::vector<std::string>&, const mmm::vec2&>,
    sol::types<const std::vector<std::string>&,
               const mmm::vec2&,
               const Menu::MenuSettings&>> menuCtors;

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
    sol::base_classes, sol::bases<class GUI>());

  module.set_usertype("Menu", menuType);
  sol::table menu = module["Menu"];

  menu["Settings"] = openMenuSettings(state);
  menu["Orientation"] = openMenuOrientation(state);

  // clang-format on
  return menu;
}

/**
 * @brief
 *   It sometimes makes sense to have only the MenuSettings class
 *   available from Lua. This function only exposes that class
 *
 * @param state
 *
 * @return
 */
sol::table LuaLib::GUI::openMenuSettings(sol::this_state state) {
  sol::state_view lua(state);
  sol::table      module = lua.create_table();

  // Enable the menu settings
  // clang-format off
  sol::constructors<
    sol::types<float, float, int, int>,
    sol::types<float, float, int>,
    sol::types<float, float>,
    sol::types<float>,
    sol::types<>> menuSettingsCtors;

  sol::usertype<Menu::MenuSettings> menuSettings(menuSettingsCtors,
    "size"  , &Menu::MenuSettings::size,
    "offset", &Menu::MenuSettings::offset,
    "ori"   , &Menu::MenuSettings::ori,
    "color" , &Menu::MenuSettings::color);

  module.set_usertype("MenuSettings", menuSettings);

  return module["MenuSettings"];
}

/**
 * @brief
 *   Since it is sometimes useful to have only the MenuOrientation
 *   available in Lua, this function exposes those enum types
 *
 * @param state
 *
 * @return
 */
sol::table LuaLib::GUI::openMenuOrientation(sol::this_state state) {
  sol::state_view lua(state);
  sol::table      module = lua.create_table();

  // Enable the menu settings
  // clang-format off
  module["Horizontal"] = Menu::HORIZONTAL;
  module["Vertical"]   = Menu::VERTICAL;

  return module;
}

/**
 * @brief
 *   Exposes the Slider class to Lua as a module
 *
 * @param state
 *
 * @return
 */
sol::table LuaLib::GUI::openSlider(sol::this_state state) {
  sol::state_view lua(state);
  sol::table      module = lua.create_table();

  sol::constructors<sol::types<const mmm::vec2&>,
                    sol::types<const mmm::vec2&, float>,
                    sol::types<const mmm::vec2&, float, const std::string&>>
    sliderCtor;

  // clang-format off
  sol::usertype<Slider> dropdownType(sliderCtor,
    // Inherited from GUI
    // Overloading functions
    "isVisible"  , sol::overload((bool(Slider::*)() const) &Slider::isVisible,
                                 (void(Slider::*)(bool)) &Slider::isVisible),

    "isClickable", sol::overload((bool(Slider::*)() const) &Slider::isClickable,
                                 (void(Slider::*)(bool)) &Slider::isClickable),

    "isAnimating", sol::overload((bool(Slider::*)() const) &Slider::isAnimating,
                                 (void(Slider::*)(bool)) &Slider::isAnimating),

    "isMinimized", sol::overload((bool(Slider::*)() const) &Slider::isMinimized,
                                 (void(Slider::*)(bool)) &Slider::isMinimized),

    "isMouseOver", sol::overload((bool(Slider::*)() const) &Slider::isMouseOver,
                                 (void(Slider::*)(bool)) &Slider::isMouseOver),

    "hasChanged" , sol::overload((bool(Slider::*)() const) &Slider::hasChanged,
                                 (void(Slider::*)(bool)) &Slider::hasChanged),

    "update"               , &Slider::update,
    "isInside"             , &Slider::isInside,
    "input"                , &Slider::input,
    "setInputHandler"      , &Slider::setInputHandler,
    "defaultInputHandler"  , &Slider::defaultInputHandler,
    "box"                  , &Slider::box,
    "position"             , &Slider::position,
    "size"                 , &Slider::size,
    "offset"               , &Slider::offset,
    "setSize"              , &Slider::setSize,

    // Functions specific to Slider
    "value"     , &Slider::value,
    "moveSlider", &Slider::moveSlider,
    "setSlider" , &Slider::setSlider,
    "draw"      , &Slider::draw,
    sol::base_classes, sol::bases<class GUI>());

  module.set_usertype("Slider", dropdownType);
  // clang-format on

  return module["Slider"];
}

sol::table LuaLib::GUI::openText(sol::this_state state) {
  sol::state_view lua(state);
  sol::table      module = lua.create_table();

  // clang-format off

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
    sol::base_classes, sol::bases<class GUI>());

  module.set_usertype("Text", type);

  sol::table text = module["Text"];
  // clang-format on

  text["Color"] = openTextColor(state);
  text["Style"] = openTextStyle(state);

  return text;
}

sol::table LuaLib::GUI::openTextColor(sol::this_state state) {
  sol::state_view lua(state);
  sol::table      module = lua.create_table();

  module["Black"]  = Text::BLACK;
  module["White"]  = Text::WHITE;
  module["Red"]    = Text::RED;
  module["Green"]  = Text::GREEN;
  module["Blue"]   = Text::BLUE;
  module["Yellow"] = Text::YELLOW;

  return module;
}

sol::table LuaLib::GUI::openTextStyle(sol::this_state state) {
  sol::state_view lua(state);
  sol::table      module = lua.create_table();

  module["Bold"]      = Text::BOLD;
  module["Underline"] = Text::UNDERLINE;
  module["Italic"]    = Text::ITALIC;

  return module;
}

sol::table LuaLib::GUI::openWindow(sol::this_state state) {
  sol::state_view lua(state);
  sol::table      module = lua.create_table();

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
    sol::base_classes, sol::bases<class GUI>());

  module.set_usertype("Window", windowType);
  // clang-format on

  return module["Window"];
}

/**
 * @brief
 *   If one uses a lot of GUI elements, it might be reasonable to import
 *   the entire GUI toolkit instead of importing each and every type
 *   that you're using.
 *
 *   This function allows that by creating a table with each
 *   type attached to it by the name of the class.
 *
 * @param state
 *
 * @return
 */
sol::table LuaLib::GUI::openGUI(sol::this_state state) {
  sol::state_view lua(state);
  sol::table      module = lua.create_table();

  module["Dropdown"] = openDropdown(state);
  module["Inputbox"] = openInputbox(state);
  module["Menu"]     = openMenu(state);
  module["Text"]     = openText(state);
  module["Window"]   = openWindow(state);

  return module;
}

/**
 * @brief
 *   Instead of exporting the entire Event class, this exports the
 *   Input::Event::Type enums.
 *
 * @param state
 *
 * @return
 */
sol::table LuaLib::Input::openEventType(sol::this_state state) {
  sol::state_view lua(state);
  sol::table      module = lua.create_table();

  module["Consumed"]       = ::Input::Event::Type::Consumed;
  module["MouseMovement"]  = ::Input::Event::Type::MouseMovement;
  module["MousePress"]     = ::Input::Event::Type::MousePress;
  module["MouseRelease"]   = ::Input::Event::Type::MouseRelease;
  module["MouseScroll"]    = ::Input::Event::Type::MouseScroll;
  module["KeyPress"]       = ::Input::Event::Type::KeyPress;
  module["KeyRelease"]     = ::Input::Event::Type::KeyRelease;
  module["CharacterInput"] = ::Input::Event::Type::CharacterInput;

  return module;
}

/**
 * @brief
 *   This exports the entire Event class to Lua. It should
 *   not be possible to create an instance of Event as that
 *   is only for the Engine to do.
 *
 *   The Event class also has `Type` enum added to it.
 *
 * @param state
 *
 * @return
 */
sol::table LuaLib::Input::openEvent(sol::this_state state) {
  sol::state_view lua(state);
  sol::table      module = lua.create_table();

  // clang-format off
  // Add all the functions that the Event class exposes.
  // The idea is that you're never allowed to do `Event.new()`, since
  // that is only done by the Engine.
  // clang-format off
  module.new_usertype<::Input::Event>("Event",
    "new"            , sol::no_constructor,
    "type"           , &::Input::Event::type,
    "hasAlt"         , &::Input::Event::hasAlt,
    "hasCtrl"        , &::Input::Event::hasCtrl,
    "hasSuper"       , &::Input::Event::hasSuper,
    "hasShift"       , &::Input::Event::hasShift,
    "key"            , &::Input::Event::key,
    "button"         , &::Input::Event::button,
    "isAction"       , &::Input::Event::isAction,
    "isKeyHeldDown"  , &::Input::Event::isKeyHeldDown,
    "keyPressed"     , &::Input::Event::keyPressed,
    "buttonPressed"  , &::Input::Event::buttonPressed,
    "position"       , &::Input::Event::position,
    "scrollLeft"     , &::Input::Event::scrollLeft,
    "scrollRight"    , &::Input::Event::scrollRight,
    "scrollUp"       , &::Input::Event::scrollUp,
    "scrollDown"     , &::Input::Event::scrollDown,
    "hasBeenHandled" , &::Input::Event::hasBeenHandled,
    "character"      , &::Input::Event::character,
    "prevType"       , &::Input::Event::prevType,
    "state"          , &::Input::Event::state,
    "sendStateChange", &::Input::Event::sendStateChange,
    "stopPropgation" , &::Input::Event::stopPropgation);

  sol::table event = module["Event"];

  event["Type"] = openEventType(state);

  // clang-format on

  return event;
}

/**
 * @brief
 *   Goes through all the keys defined in Input::keyMap and adds
 *   them to module.
 *
 * @param state
 *
 * @return
 */
sol::table LuaLib::Input::openKeys(sol::this_state state) {
  sol::state_view lua(state);
  sol::table      module = lua.create_table();

  // Add all the keys that defined on Input
  for (auto& a : ::Input::keyMap) {
    module[a.first] = a.second;
  }

  module.set_function("toString",
                      [](int key) { return ::Input::glfwKeyToString(key); });

  return module;
}

/**
 * @brief
 *   Loads the entire Input library when required. Currently,
 *   this is only Event and its Type
 *
 * @param state
 *
 * @return
 */
sol::table LuaLib::Input::openInput(sol::this_state state) {
  sol::state_view lua(state);
  sol::table      module = lua.create_table();

  module["Event"] = openEvent(state);
  module["Keys"]  = openKeys(state);

  return module;
}

/**
 * @brief
 *   Exposes the mmm::vec2 class to Lua
 *
 * @param state
 *
 * @return
 */
sol::table LuaLib::Math::openVec2(sol::this_state state) {
  sol::state_view lua(state);
  sol::table      module = lua.create_table();

  sol::constructors<sol::types<>,
                    sol::types<int>,
                    sol::types<float>,
                    sol::types<int, int>,
                    sol::types<float, float>>
                           ctor;
  sol::usertype<mmm::vec2> type(ctor, "x", &mmm::vec2::x, "y", &mmm::vec2::y);

  module.set_usertype("vec2", type);

  return module["vec2"];
}

/**
 * @brief
 *   Exposes the mmm::vec3 class to Lua, with 6 constructors,
 *
 *   - no variables
 *   - 1 int
 *   - 1 float
 *   - 3 float
 *   - 3 int
 *   - vec2, float
 *   - vec2, int
 *
 * @param state
 *
 * @return
 */
sol::table LuaLib::Math::openVec3(sol::this_state state) {
  sol::state_view lua(state);
  sol::table      module = lua.create_table();

  sol::constructors<sol::types<>,
                    sol::types<int>,
                    sol::types<float>,
                    sol::types<float, float, float>,
                    sol::types<int, int, int>,
                    sol::types<mmm::vec2, float>,
                    sol::types<mmm::vec2, int>>
    ctor;
  sol::usertype<mmm::vec3>
    type(ctor, "x", &mmm::vec3::x, "y", &mmm::vec3::y, "z", &mmm::vec3::z);

  module.set_usertype("vec3", type);

  return module["vec3"];
}

/**
 * @brief
 *   Loads the entire Math library
 *
 * @param state
 *
 * @return
 */
sol::table LuaLib::Math::openMath(sol::this_state state) {
  sol::state_view lua(state);
  sol::table      module = lua.create_table();

  module["vec2"] = openVec2(state);
  module["vec3"] = openVec3(state);

  return module;
}

/**
 * @brief
 *   Exposes the Rectangle class to Lua. This class contains two vec2's,
 *   one with the location of the top left corner and one with the size.
 *
 *   It also exposes several functions to retrieve the size, topleft or
 *   bottomleft.
 *
 * @param state
 *
 * @return
 */
sol::table LuaLib::Shape::openRectangle(sol::this_state state) {
  sol::state_view lua(state);
  sol::table      module = lua.create_table();

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

  module.set_usertype("Rectangle", type);
  // clang-format on

  return module["Rectangle"];
}

/**
 * @brief
 *   Loads all the Shapes into one object that is returned
 *   when Shape is required
 *
 * @param state
 *
 * @return
 */
sol::table LuaLib::Shape::openShape(sol::this_state state) {
  sol::state_view lua(state);
  sol::table      module = lua.create_table();

  module["Rectangle"] = openRectangle(state);

  return module;
}

/**
 * @brief
 *   Exposes the base Drawable3D class with most of its functions
 *
 * @param state
 *
 * @return
 */
sol::table LuaLib::Drawable::openDrawable3D(sol::this_state state) {
  sol::state_view lua(state);
  sol::table      module = lua.create_table();

  // clang-format off
  sol::constructors<> ctor;

  sol::usertype<Drawable3D> type(ctor,
    "updateFromPhysics", &Drawable3D::updateFromPhysics,
    "hasPhysics", &Drawable3D::hasPhysics,
    "position", &Drawable3D::position,
    "moveTo", sol::overload(
        (void(Drawable3D::*)(float, float, float)) &Drawable3D::moveTo,
        (void(Drawable3D::*)(const mmm::vec3&)) &Drawable3D::moveTo),
    "rotate", sol::overload(
        (void(Drawable3D::*)(float, float, float, float)) &Drawable3D::rotate,
        (void(Drawable3D::*)(const mmm::vec3&, float)) &Drawable3D::rotate),
    "weight", &Drawable3D::weight,
    "collisionGroup", &Drawable3D::collisionGroup,
    "collisionMask", &Drawable3D::collisionMask,
    "setCollisionGroup", &Drawable3D::setCollisionGroup,
    "setCollisionMask", &Drawable3D::setCollisionMask);

  module.set_usertype("Drawable3D", type);

  return module["Drawable3D"];
}

/**
 * @brief
 *   Exposes the Spider class, which currently has no additional functions
 *   besides the ones that are located on Drawable3D
 *
 * @param state
 *
 * @return
 */
sol::table LuaLib::Drawable::openSpider(sol::this_state state) {
  sol::state_view lua(state);
  sol::table      module = lua.create_table();

  // clang-format off
  sol::constructors<> ctor;

  sol::usertype<Spider> type(ctor,
      sol::base_classes, sol::bases<Drawable3D, class Drawable>());

  module.set_usertype("Spider", type);

  return module["Spider"];
}

/**
 * @brief
 *   Loads the States enum class
 *
 * @param state
 *
 * @return
 */
sol::table LuaLib::State::openStateTypes(sol::this_state state) {
  sol::state_view lua(state);
  sol::table      module = lua.create_table();

  module["QuitAll"]          = States::QuitAll;
  module["Quit"]             = States::Quit;
  module["Init"]             = States::Init;
  module["OptionsMenuClose"] = States::OptionsMenuClose;
  module["OptionsMenuOpen"]  = States::OptionsMenuOpen;
  module["PauseMenuOpen"]    = States::PauseMenuOpen;
  module["PauseMenuClose"]   = States::PauseMenuClose;
  module["MainMenu"]         = States::MainMenu;
  module["Game"]             = States::Game;
  module["MasterThesis"]     = States::MasterThesis;
  module["Refresh"]          = States::Refresh;
  module["WinRefresh"]       = States::WinRefresh;
  module["NoChange"]         = States::NoChange;
  module["LuaReload"]        = States::LuaReload;

  return module;
}

/**
 * @brief
 *   Exposes the State base class, which cannot be constructed, but exposes
 *   some functions to retrieve the currently drawn/stored objects
 *
 * @param state
 *
 * @return
 */
sol::table LuaLib::State::openStateClass(sol::this_state state) {
  sol::state_view lua(state);
  sol::table      module = lua.create_table();

  sol::constructors<> ctor;

  sol::usertype<class State> type(ctor,
    "drawable", &::State::drawable,
    "gui", &::State::gui,
    "numDrawables", &::State::numDrawables,
    "numGUIs", &::State::numGUIs);

  module.set_usertype("State", type);

  return module["State"];
}

/**
 * @brief
 *  Loads the entire State library
 *
 * @param state
 *
 * @return
 */
sol::table LuaLib::State::openState(sol::this_state state) {
  sol::state_view lua(state);
  sol::table      module = lua.create_table();

  module["Type"] = openStateTypes(state);

  return module;
}

/**
 * @brief
 *   This defines the handler for requiring 'Util'. Utils contains
 *   a lot of useful functions that makes working with Lua and C++
 *   easier
 *
 * @param state
 *
 * @return
 */
sol::table LuaLib::Util::openUtil(sol::this_state state) {
  sol::state_view lua(state);
  sol::table      module = lua.create_table();

  module.set_function("toVectorStr",
                      [](sol::table t) -> std::vector<std::string> {
                        std::vector<std::string> s;

                        for (auto pair : t) {
                          if (pair.second.get_type() == sol::type::string) {
                            s.push_back(pair.second.as<std::string>());
                          }
                        }

                        return s;
                      });

  return module;
}

sol::table LuaLib::Learning::openLearning(sol::this_state state) {
  sol::state_view lua(state);
  sol::table      module = lua.create_table();

  module["SpiderSwarm"] = openSpiderSwarm(state);
  module["NEATParameters"] = openNEATParameters(state);
  module["openNEATSubstrate"] = openNEATSubstrate(state);
  module["SpiderSwarm"] = lua.create_named_table("DrawingMethod",
   "DrawSingleInBatch", SpiderSwarm::DrawingMethod::DrawSingleInBatch,
   "DrawAllInBatch", SpiderSwarm::DrawingMethod::DrawAllInBatch,
   "Species1", SpiderSwarm::DrawingMethod::Species1,
   "SpeciesLeaders", SpiderSwarm::DrawingMethod::SpeciesLeaders,
   "BestFitness", SpiderSwarm::DrawingMethod::BestFitness,
   "DrawAll", SpiderSwarm::DrawingMethod::DrawAll,
   "DrawNone", SpiderSwarm::DrawingMethod::DrawNone);

  return module;
}

sol::table LuaLib::Learning::openSpiderSwarm(sol::this_state state) {
  sol::state_view lua(state);
  sol::table      module = lua.create_table();

  sol::constructors<> ctor;

  sol::usertype<SpiderSwarm> type(ctor,
    "save", &SpiderSwarm::save,
    "load", &SpiderSwarm::load,
    "parameters", &SpiderSwarm::parameters,
    "substrate", &SpiderSwarm::substrate,
    "restart", &SpiderSwarm::restart,
    "setup", &SpiderSwarm::setup,
    "start", &SpiderSwarm::start,
    "stop", &SpiderSwarm::stop,
    "setDrawingMethod", &SpiderSwarm::setDrawingMethod,
    "runGenome", &SpiderSwarm::runGenome,
    "runBestGenome", &SpiderSwarm::runBestGenome,
    "loadGenome", &SpiderSwarm::loadGenome,
    "disableDrawing", &SpiderSwarm::disableDrawing,
    "enableDrawing", &SpiderSwarm::enableDrawing,
    "toggleDrawANN", &SpiderSwarm::toggleDrawANN,
    "currentDuration", &SpiderSwarm::currentDuration);

  module.set_usertype("SpiderSwarm", type);

  return module["SpiderSwarm"];
}

sol::table LuaLib::Learning::openNEATSubstrate(sol::this_state state) {
  sol::state_view lua(state);
  sol::table      module = lua.create_table();

  sol::constructors<> ctor;

  sol::usertype<NEAT::Substrate> type(ctor,
    "m_leaky", &NEAT::Substrate::m_leaky,
    "m_allow_input_hidden_links", &NEAT::Substrate::m_allow_input_hidden_links,
    "m_allow_input_output_links", &NEAT::Substrate::m_allow_input_output_links,
    "m_allow_hidden_hidden_links", &NEAT::Substrate::m_allow_hidden_hidden_links,
    "m_allow_hidden_output_links", &NEAT::Substrate::m_allow_hidden_output_links,
    "m_allow_output_hidden_links", &NEAT::Substrate::m_allow_output_hidden_links,
    "m_allow_output_output_links", &NEAT::Substrate::m_allow_output_output_links,
    "m_allow_looped_hidden_links", &NEAT::Substrate::m_allow_looped_hidden_links,
    "m_allow_looped_output_links", &NEAT::Substrate::m_allow_looped_output_links,
    "m_query_weights_only", &NEAT::Substrate::m_query_weights_only,
    "m_hidden_nodes_activation", &NEAT::Substrate::m_hidden_nodes_activation,
    "m_output_nodes_activation", &NEAT::Substrate::m_output_nodes_activation,
    "m_max_weight_and_bias", &NEAT::Substrate::m_max_weight_and_bias,
    "m_min_time_const", &NEAT::Substrate::m_min_time_const,
    "m_max_time_const", &NEAT::Substrate::m_max_time_const);

  module.set_usertype("NEATSubstrate", type);

  return module["NEATSubstrate"];
}

sol::table LuaLib::Learning::openNEATParameters(sol::this_state state) {
  sol::state_view lua(state);
  sol::table      module = lua.create_table();

  sol::constructors<> ctor;

  sol::usertype<NEAT::Parameters> type(ctor,
    "PopulationSize", &NEAT::Parameters::PopulationSize,
    "DynamicCompatibility", &NEAT::Parameters::DynamicCompatibility,
    "MinSpecies", &NEAT::Parameters::MinSpecies,
    "MaxSpecies", &NEAT::Parameters::MaxSpecies,
    "InnovationsForever", &NEAT::Parameters::InnovationsForever,
    "AllowClones", &NEAT::Parameters::AllowClones,
    "YoungAgeTreshold", &NEAT::Parameters::YoungAgeTreshold,
    "YoungAgeFitnessBoost", &NEAT::Parameters::YoungAgeFitnessBoost,
    "SpeciesMaxStagnation", &NEAT::Parameters::SpeciesMaxStagnation,
    "StagnationDelta", &NEAT::Parameters::StagnationDelta,
    "OldAgeTreshold", &NEAT::Parameters::OldAgeTreshold,
    "OldAgePenalty", &NEAT::Parameters::OldAgePenalty,
    "DetectCompetetiveCoevolutionStagnation", &NEAT::Parameters::DetectCompetetiveCoevolutionStagnation,
    "KillWorstSpeciesEach", &NEAT::Parameters::KillWorstSpeciesEach,
    "KillWorstAge", &NEAT::Parameters::KillWorstAge,
    "SurvivalRate", &NEAT::Parameters::SurvivalRate,
    "CrossoverRate", &NEAT::Parameters::CrossoverRate,
    "OverallMutationRate", &NEAT::Parameters::OverallMutationRate,
    "InterspeciesCrossoverRate", &NEAT::Parameters::InterspeciesCrossoverRate,
    "MultipointCrossoverRate", &NEAT::Parameters::MultipointCrossoverRate,
    "RouletteWheelSelection", &NEAT::Parameters::RouletteWheelSelection,
    "TournamentSize", &NEAT::Parameters::TournamentSize,
    "EliteFraction", &NEAT::Parameters::EliteFraction,
    "PhasedSearching", &NEAT::Parameters::PhasedSearching,
    "DeltaCoding", &NEAT::Parameters::DeltaCoding,
    "SimplifyingPhaseMPCTreshold", &NEAT::Parameters::SimplifyingPhaseMPCTreshold,
    "SimplifyingPhaseStagnationTreshold", &NEAT::Parameters::SimplifyingPhaseStagnationTreshold,
    "ComplexityFloorGenerations", &NEAT::Parameters::ComplexityFloorGenerations,
    "NoveltySearch_K", &NEAT::Parameters::NoveltySearch_K,
    "NoveltySearch_P_min", &NEAT::Parameters::NoveltySearch_P_min,
    "NoveltySearch_Dynamic_Pmin", &NEAT::Parameters::NoveltySearch_Dynamic_Pmin,
    "NoveltySearch_No_Archiving_Stagnation_Treshold", &NEAT::Parameters::NoveltySearch_No_Archiving_Stagnation_Treshold,
    "NoveltySearch_Pmin_lowering_multiplier", &NEAT::Parameters::NoveltySearch_Pmin_lowering_multiplier,
    "NoveltySearch_Pmin_min", &NEAT::Parameters::NoveltySearch_Pmin_min,
    "NoveltySearch_Quick_Archiving_Min_Evaluations", &NEAT::Parameters::NoveltySearch_Quick_Archiving_Min_Evaluations,
    "NoveltySearch_Pmin_raising_multiplier", &NEAT::Parameters::NoveltySearch_Pmin_raising_multiplier,
    "NoveltySearch_Recompute_Sparseness_Each", &NEAT::Parameters::NoveltySearch_Recompute_Sparseness_Each,
    "MutateAddNeuronProb", &NEAT::Parameters::MutateAddNeuronProb,
    "SplitRecurrent", &NEAT::Parameters::SplitRecurrent,
    "SplitLoopedRecurrent", &NEAT::Parameters::SplitLoopedRecurrent,
    "NeuronTries", &NEAT::Parameters::NeuronTries,
    "MutateAddLinkProb", &NEAT::Parameters::MutateAddLinkProb,
    "MutateAddLinkFromBiasProb", &NEAT::Parameters::MutateAddLinkFromBiasProb,
    "MutateRemLinkProb", &NEAT::Parameters::MutateRemLinkProb,
    "MutateRemSimpleNeuronProb", &NEAT::Parameters::MutateRemSimpleNeuronProb,
    "LinkTries", &NEAT::Parameters::LinkTries,
    "RecurrentProb", &NEAT::Parameters::RecurrentProb,
    "RecurrentLoopProb", &NEAT::Parameters::RecurrentLoopProb,
    "MutateWeightsProb", &NEAT::Parameters::MutateWeightsProb,
    "MutateWeightsSevereProb", &NEAT::Parameters::MutateWeightsSevereProb,
    "WeightMutationRate", &NEAT::Parameters::WeightMutationRate,
    "WeightMutationMaxPower", &NEAT::Parameters::WeightMutationMaxPower,
    "WeightReplacementMaxPower", &NEAT::Parameters::WeightReplacementMaxPower,
    "MaxWeight", &NEAT::Parameters::MaxWeight,
    "MutateActivationAProb", &NEAT::Parameters::MutateActivationAProb,
    "MutateActivationBProb", &NEAT::Parameters::MutateActivationBProb,
    "ActivationAMutationMaxPower", &NEAT::Parameters::ActivationAMutationMaxPower,
    "ActivationBMutationMaxPower", &NEAT::Parameters::ActivationBMutationMaxPower,
    "TimeConstantMutationMaxPower", &NEAT::Parameters::TimeConstantMutationMaxPower,
    "BiasMutationMaxPower", &NEAT::Parameters::BiasMutationMaxPower,
    "MinActivationA", &NEAT::Parameters::MinActivationA,
    "MaxActivationA", &NEAT::Parameters::MaxActivationA,
    "MinActivationB", &NEAT::Parameters::MinActivationB,
    "MaxActivationB", &NEAT::Parameters::MaxActivationB,
    "MutateNeuronActivationTypeProb", &NEAT::Parameters::MutateNeuronActivationTypeProb,
    "ActivationFunction_SignedSigmoid_Prob", &NEAT::Parameters::ActivationFunction_SignedSigmoid_Prob,
    "ActivationFunction_UnsignedSigmoid_Prob", &NEAT::Parameters::ActivationFunction_UnsignedSigmoid_Prob,
    "ActivationFunction_Tanh_Prob", &NEAT::Parameters::ActivationFunction_Tanh_Prob,
    "ActivationFunction_TanhCubic_Prob", &NEAT::Parameters::ActivationFunction_TanhCubic_Prob,
    "ActivationFunction_SignedStep_Prob", &NEAT::Parameters::ActivationFunction_SignedStep_Prob,
    "ActivationFunction_UnsignedStep_Prob", &NEAT::Parameters::ActivationFunction_UnsignedStep_Prob,
    "ActivationFunction_SignedGauss_Prob", &NEAT::Parameters::ActivationFunction_SignedGauss_Prob,
    "ActivationFunction_UnsignedGauss_Prob", &NEAT::Parameters::ActivationFunction_UnsignedGauss_Prob,
    "ActivationFunction_Abs_Prob", &NEAT::Parameters::ActivationFunction_Abs_Prob,
    "ActivationFunction_SignedSine_Prob", &NEAT::Parameters::ActivationFunction_SignedSine_Prob,
    "ActivationFunction_UnsignedSine_Prob", &NEAT::Parameters::ActivationFunction_UnsignedSine_Prob,
    "ActivationFunction_Linear_Prob", &NEAT::Parameters::ActivationFunction_Linear_Prob,
    "ActivationFunction_Relu_Prob", &NEAT::Parameters::ActivationFunction_Relu_Prob,
    "ActivationFunction_Softplus_Prob", &NEAT::Parameters::ActivationFunction_Softplus_Prob,
    "MutateNeuronTimeConstantsProb", &NEAT::Parameters::MutateNeuronTimeConstantsProb,
    "MutateNeuronBiasesProb", &NEAT::Parameters::MutateNeuronBiasesProb,
    "MinNeuronTimeConstant", &NEAT::Parameters::MinNeuronTimeConstant,
    "MaxNeuronTimeConstant", &NEAT::Parameters::MaxNeuronTimeConstant,
    "MinNeuronBias", &NEAT::Parameters::MinNeuronBias,
    "MaxNeuronBias", &NEAT::Parameters::MaxNeuronBias,
    "DisjointCoeff", &NEAT::Parameters::DisjointCoeff,
    "ExcessCoeff", &NEAT::Parameters::ExcessCoeff,
    "ActivationADiffCoeff", &NEAT::Parameters::ActivationADiffCoeff,
    "ActivationBDiffCoeff", &NEAT::Parameters::ActivationBDiffCoeff,
    "WeightDiffCoeff", &NEAT::Parameters::WeightDiffCoeff,
    "TimeConstantDiffCoeff", &NEAT::Parameters::TimeConstantDiffCoeff,
    "BiasDiffCoeff", &NEAT::Parameters::BiasDiffCoeff,
    "ActivationFunctionDiffCoeff", &NEAT::Parameters::ActivationFunctionDiffCoeff,
    "CompatTreshold", &NEAT::Parameters::CompatTreshold,
    "MinCompatTreshold", &NEAT::Parameters::MinCompatTreshold,
    "CompatTresholdModifier", &NEAT::Parameters::CompatTresholdModifier,
    "CompatTreshChangeInterval_Generations", &NEAT::Parameters::CompatTreshChangeInterval_Generations,
    "CompatTreshChangeInterval_Evaluations", &NEAT::Parameters::CompatTreshChangeInterval_Evaluations,
    "DivisionThreshold", &NEAT::Parameters::DivisionThreshold,
    "VarianceThreshold", &NEAT::Parameters::VarianceThreshold,
    "BandThreshold", &NEAT::Parameters::BandThreshold,
    "InitialDepth", &NEAT::Parameters::InitialDepth,
    "MaxDepth", &NEAT::Parameters::MaxDepth,
    "IterationLevel", &NEAT::Parameters::IterationLevel,
    "CPPN_Bias", &NEAT::Parameters::CPPN_Bias,
    "Width", &NEAT::Parameters::Width,
    "Height", &NEAT::Parameters::Height,
    "Qtree_X", &NEAT::Parameters::Qtree_X,
    "Qtree_Y", &NEAT::Parameters::Qtree_Y,
    "Leo", &NEAT::Parameters::Leo,
    "LeoThreshold", &NEAT::Parameters::LeoThreshold,
    "LeoSeed", &NEAT::Parameters::LeoSeed,
    "GeometrySeed", &NEAT::Parameters::GeometrySeed);

  module.set_usertype("NEATParameters", type);

  return module["NEATParameters"];
}
