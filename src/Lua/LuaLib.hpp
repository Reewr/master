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
 *
 * The namespace in this file represents how it will be exported to Lua,
 * in some sense.
 *
 * For instance:
 *
 * - `LuaLib::openConsole` will be exported as `Console`
 * - `LuaLib::GUI::openDropdown` will be exported as `GUI.Dropdown`
 *
 * In addition, to make things simpler for the users of the API, the namespaces
 * should also make one function available that exposes the entire namespace as
 * one `sol::table`, as in the case of LuaLib::GUI::openGUI.
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
    // Exposes the Input::Event class
    sol::table openEvent(sol::this_state state);

    // Exposes the Input::Event::Type enums
    sol::table openEventType(sol::this_state state);

    // Exposes all the keys from the Input class
    sol::table openKeys(sol::this_state state);

    // Exposes all the Input classes
    sol::table openInput(sol::this_state state);
  }

  namespace Math {
    // exposes the vec2 class
    sol::table openVec2(sol::this_state state);

    // exposes the vec3 class
    sol::table openVec3(sol::this_state state);

    // exposes all the objects defined above
    sol::table openMath(sol::this_state state);
  }

  namespace Shape {
    // Exposes the Rectangle class
    sol::table openRectangle(sol::this_state state);

    // Exposes all objects defined above
    sol::table openShape(sol::this_state state);
  }

  namespace Drawable {
    // Exposes the Drawable3D class to Lua
    sol::table openDrawable3D(sol::this_state state);

    // Exposes the spider class to Lua
    sol::table openSpider(sol::this_state state);
  }

  namespace State {
    // Exposes the States enums
    sol::table openStateTypes(sol::this_state state);

    // Exposes the state base class
    sol::table openStateClass(sol::this_state state);

    // Exposes all the State functions above
    sol::table openState(sol::this_state state);
  }

  namespace Util {
    // Util exposes neat utility functions
    // that helps working with Lua and C++
    sol::table openUtil(sol::this_state state);
  }

  namespace Learning {
    sol::table openLearning(sol::this_state state);
    sol::table openSpiderSwarm(sol::this_state state);
  }
}
