#pragma once

#include <sol.hpp>

/**
 * This file exposes all of the Lua API. It is contained in a single compilation
 * unit to decrease the compile time.
 *
 * All the functions should follow the name `openSOMENAME` where `SOMENAME`
 * is a unique name for the resource that they are opening.
 *
 * The logic is that each of these function is suppose to be requirable
 * through Lua in some way or another. The name that they are given
 * in Lua is decided by `Lua.hpp`
 *
 * Feel free to namespace things under this namespace as you would in a normal
 * folder directory
 */
namespace LuaLib {

  // Exposes Console/Console.hpp
  sol::table openConsole(sol::this_state state);

  // Exposes the entire CFG class
  sol::table openCFG(sol::this_state state);

  namespace GUI {
    // Exposes the Dropdown class
    sol::table openDropdown(sol::this_state state);

    // Exposes the Inputbox class
    sol::table openInputbox(sol::this_state state);

    // Exposes the Menu class and its enum types
    sol::table openMenu(sol::this_state state);
    sol::table openMenuSettings(sol::this_state state);
    sol::table openMenuOrientation(sol::this_state state);

    // Exposes the Text class and its enum types
    sol::table openText(sol::this_state state);
    sol::table openTextColor(sol::this_state state);
    sol::table openTextStyle(sol::this_state state);

    // Exposes the slider class
    sol::table openSlider(sol::this_state state);

    // Exposes the window class
    sol::table openWindow(sol::this_state state);

    // Exposes the entire GUI toolkit
    sol::table openGUI(sol::this_state state);
  }

  namespace Input {
    sol::table openEvent(sol::this_state state);
    sol::table openEventType(sol::this_state state);
    sol::table openInput(sol::this_state state);
  }

  namespace Math {
    sol::table openVec2(sol::this_state state);
    sol::table openMath(sol::this_state state);
  }

  namespace Shape {
    sol::table openRectangle(sol::this_state state);
    sol::table openShape(sol::this_state state);
  }

  namespace State {
    sol::table openStateTypes(sol::this_state state);
  }
}
