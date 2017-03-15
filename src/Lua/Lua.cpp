#include "Lua.hpp"

#include "../Console/Console.hpp"
#include "../Engine.hpp"
#include "../Utils/CFG.hpp"
#include "../Utils/Utils.hpp"

#include <sol.hpp>

#include "LuaLib.hpp"

/**
 * @brief
 *   Checks if flat b exists in a
 *
 * @param a
 * @param b
 *
 * @return
 */
bool hasFlag(Lua::Lib::Native a, Lua::Lib::Native b) {
  return int(a & b) != 0;
}

/**
 * @brief
 *   Checks if flat b exists in a
 *
 * @param a
 * @param b
 *
 * @return
 */
bool hasFlag(Lua::Lib::Engine a, Lua::Lib::Engine b) {
  return int(a & b) != 0;
}

/**
 * @brief
 *   Compare two Typenames structures, returning
 *   a result so that they are sorted alphabetically.
 *
 * @param t1
 * @param t2
 *
 * @return
 */
bool compare_by_word(const Lua::TypePair& t1, const Lua::TypePair t2) {
  return t1.first < t2.first;
}

namespace Lua {

  /**
   * @brief
   *   Returns the name of a type, "Undef" if a type cannot
   *   be matched. The type names are common names for variables,
   *   such as 'Num' for 'Number'
   *
   * @param t
   *
   * @return
   */
  std::string solTypetoStr(sol::type t) {
    switch (t) {
      case sol::type::none:
        return "none";
        break;
      case sol::type::nil:
        return "nil";
        break;
      case sol::type::boolean:
        return "Bool";
        break;
      case sol::type::lightuserdata:
        return "ludata";
        break;
      case sol::type::number:
        return "Num";
        break;
      case sol::type::string:
        return "Str";
        break;
      case sol::type::table:
        return "Obj";
        break;
      case sol::type::function:
        return "Fn";
        break;
      case sol::type::userdata:
        return "udata";
        break;
      case sol::type::thread:
        return "thread";
        break;
      default:
        return "Undef";
    }
  }

  /**
   * @brief
   *   Initializes the Lua class, creating the Lua State and exporting
   *   several C++ objects to Lua.
   */
  Lua::Lua(CFG* c, Lib::Native n, Lib::Engine e)
      : Logging::Log("LuaEngine")
      , mConsole(nullptr)
      , mCFG(c)
      , mNativeLibraries(n)
      , mEngineLibraries(e) {
    reInitialize();
  }

  Lua::~Lua() {}

  /**
   * @brief
   *   Loads all the libraries that have been specified in
   *   the bitcode sent to the function
   *
   * @param nativeLib
   */
  void Lua::openLibraries(Lib::Native nativeLib) {
    if (hasFlag(nativeLib, Lib::Native::Base)) {
      mLog->debug("Loading Native :: base");
      engine.open_libraries(sol::lib::base);
    }

    if (hasFlag(nativeLib, Lib::Native::Package)) {
      mLog->debug("Loading Native :: package");
      engine.open_libraries(sol::lib::package);
    }

    if (hasFlag(nativeLib, Lib::Native::Coroutine)) {
      mLog->debug("Loading Native :: coroutine");
      engine.open_libraries(sol::lib::coroutine);
    }

    if (hasFlag(nativeLib, Lib::Native::String)) {
      mLog->debug("Loading Native :: string");
      engine.open_libraries(sol::lib::string);
    }

    if (hasFlag(nativeLib, Lib::Native::OS)) {
      mLog->debug("Loading Native :: os");
      engine.open_libraries(sol::lib::os);
    }

    if (hasFlag(nativeLib, Lib::Native::Math)) {
      mLog->debug("Loading Native :: math");
      engine.open_libraries(sol::lib::math);
    }

    if (hasFlag(nativeLib, Lib::Native::Table)) {
      mLog->debug("Loading Native :: table");
      engine.open_libraries(sol::lib::table);
    }

    if (hasFlag(nativeLib, Lib::Native::Debug)) {
      mLog->debug("Loading Native :: debug");
      engine.open_libraries(sol::lib::debug);
    }

    if (hasFlag(nativeLib, Lib::Native::Bit32)) {
      mLog->debug("Loading Native :: bit32");
      engine.open_libraries(sol::lib::bit32);
    }

    if (hasFlag(nativeLib, Lib::Native::IO)) {
      mLog->debug("Loading Native :: io");
      engine.open_libraries(sol::lib::io);
    }

    if (hasFlag(nativeLib, Lib::Native::FFI)) {
      mLog->debug("Loading Native :: ffi");
      engine.open_libraries(sol::lib::ffi);
    }

    if (hasFlag(nativeLib, Lib::Native::JIT)) {
      mLog->debug("Loading Native :: jit");
      engine.open_libraries(sol::lib::jit);
    }

    if (hasFlag(nativeLib, Lib::Native::UTF8)) {
      mLog->debug("Loading Native :: utf8");
      engine.open_libraries(sol::lib::utf8);
    }

    if (hasFlag(nativeLib, Lib::Native::Count)) {
      mLog->debug("Loading Native :: count");
      engine.open_libraries(sol::lib::count);
    }
  }
  /**
   * @brief
   *   Loads all the libraries that have been specified in
   *   the bitcode sent to the function
   *
   * @param nativeLib
   */
  void Lua::openLibraries(Lib::Engine enginelib) {
    if (hasFlag(Lib::Engine::Console, enginelib)) {
      mLog->debug("Loading Engine :: Console");
      engine.require("Console",
                     sol::c_call<decltype(&LuaLib::openConsole),
                                 &LuaLib::openConsole>,
                     false);
    }

    if (hasFlag(Lib::Engine::Math, enginelib)) {
      mLog->debug("Loading Engine :: Math");
      engine.require("Math",
                     sol::c_call<decltype(&LuaLib::Math::openMath),
                                 &LuaLib::Math::openMath>,
                     false);
      engine.require("Math.vec2",
                     sol::c_call<decltype(&LuaLib::Math::openVec2),
                                 &LuaLib::Math::openVec2>,
                     false);
    }

    if (hasFlag(Lib::Engine::Shape, enginelib)) {
      mLog->debug("Loading Engine :: Shape");
      engine.require("Shape",
                     sol::c_call<decltype(&LuaLib::Shape::openShape),
                                 &LuaLib::Shape::openShape>,
                     false);
      engine.require("Shape.Rectangle",
                     sol::c_call<decltype(&LuaLib::Shape::openRectangle),
                                 &LuaLib::Shape::openRectangle>,
                     false);
    }

    if (hasFlag(Lib::Engine::GUI, enginelib)) {
      mLog->debug("Loading Engine :: GUI");
      engine.require("GUI",
                     sol::c_call<decltype(&LuaLib::GUI::openGUI),
                                 &LuaLib::GUI::openGUI>,
                     false);

      engine.require("GUI.Dropdown",
                     sol::c_call<decltype(&LuaLib::GUI::openDropdown),
                                 &LuaLib::GUI::openDropdown>,
                     false);

      engine.require("GUI.Text",
                     sol::c_call<decltype(&LuaLib::GUI::openText),
                                 &LuaLib::GUI::openText>,
                     false);

      engine.require("GUI.Menu",
                     sol::c_call<decltype(&LuaLib::GUI::openMenu),
                                 &LuaLib::GUI::openMenu>,
                     false);

      engine.require("GUI.Menu.Settings",
                     sol::c_call<decltype(&LuaLib::GUI::openMenuSettings),
                                 &LuaLib::GUI::openMenuSettings>,
                     false);

      engine.require("GUI.Menu.Orientation",
                     sol::c_call<decltype(&LuaLib::GUI::openMenuOrientation),
                                 &LuaLib::GUI::openMenuOrientation>,
                     false);

      engine.require("GUI.Text.Color",
                     sol::c_call<decltype(&LuaLib::GUI::openTextColor),
                                 &LuaLib::GUI::openTextColor>,
                     false);

      engine.require("GUI.Text.Style",
                     sol::c_call<decltype(&LuaLib::GUI::openTextStyle),
                                 &LuaLib::GUI::openTextStyle>,
                     false);

      engine.require("GUI.Inputbox",
                     sol::c_call<decltype(&LuaLib::GUI::openInputbox),
                                 &LuaLib::GUI::openInputbox>,
                     false);


      engine.require("GUI.Slider",
                     sol::c_call<decltype(&LuaLib::GUI::openSlider),
                                 &LuaLib::GUI::openSlider>,
                     false);

      engine.require("GUI.Window",
                     sol::c_call<decltype(&LuaLib::GUI::openWindow),
                                 &LuaLib::GUI::openWindow>,
                     false);
    }

    if (hasFlag(Lib::Engine::Input, enginelib)) {
      mLog->debug("Loading Engine :: Input");
      engine.require("Input",
                     sol::c_call<decltype(&LuaLib::Input::openInput),
                                 &LuaLib::Input::openInput>,
                     false);
      engine.require("Input.Keys",
                     sol::c_call<decltype(&LuaLib::Input::openKeys),
                                 &LuaLib::Input::openKeys>,
                     false);
      engine.require("Input.Event",
                     sol::c_call<decltype(&LuaLib::Input::openEvent),
                                 &LuaLib::Input::openEvent>,
                     false);

      engine.require("Input.Event.Type",
                     sol::c_call<decltype(&LuaLib::Input::openEventType),
                                 &LuaLib::Input::openEventType>,
                     false);
    }

    if (hasFlag(Lib::Engine::CFG, enginelib)) {
      mLog->debug("Loading Engine :: CFG");
      engine.require("CFG",
                     sol::c_call<decltype(&LuaLib::openCFG), &LuaLib::openCFG>,
                     false);
    }

    if (hasFlag(Lib::Engine::Drawable, enginelib)) {
      mLog->debug("Loading Engine :: Drawable");
      engine.require("Drawable.3D",
                     sol::c_call<decltype(&LuaLib::Drawable::openDrawable3D),
                                 &LuaLib::Drawable::openDrawable3D>,
                     false);
      engine.require("Drawable.3D.Spider",
                     sol::c_call<decltype(&LuaLib::Drawable::openSpider),
                                 &LuaLib::Drawable::openSpider>,
                     false);
    }

    if (hasFlag(Lib::Engine::State, enginelib)) {
      mLog->debug("Loading Engine :: State");
      engine.require("State",
                     sol::c_call<decltype(&LuaLib::State::openState),
                                 &LuaLib::State::openState>,
                     false);
      engine.require("State.Type",
                     sol::c_call<decltype(&LuaLib::State::openStateTypes),
                                 &LuaLib::State::openStateTypes>,
                     false);
    }

    if (hasFlag(Lib::Engine::Util, enginelib)) {
      mLog->debug("Loading Engine :: Util");
      engine.require("Util",
                     sol::c_call<decltype(&LuaLib::Util::openUtil),
                                 &LuaLib::Util::openUtil>,
                     false);
    }
  }

  /**
   * @brief
   *   This recreates the state.
   *   It will also try to load the `main.lua` file located in `./lua`.
   *
   *   This function is also used by the constructor. If used after this,
   *   it will reload everything.
   */
  void Lua::reInitialize() {
    engine = sol::state();

    openLibraries(mNativeLibraries);
    openLibraries(mEngineLibraries);

    std::string path          = engine["package"]["path"];
    std::string sep           = path.empty() ? "" : ";";
    engine["package"]["path"] = path + sep + "./lua/?.lua";
    engine["cfg"]             = mCFG;

    if (mConsole != nullptr)
      add(mConsole);

    loadFile("lua/main.lua");

    emit("reInitialize");
  }

  /**
   * @brief
   *   Tells the Lua engine to add the console to global scope
   *   as a `console` variable, making it available to Lua code.
   *
   *   It will store the pointer to the console so that when
   *   `reinitialize` is done, it can include it again.
   *
   * @param console
   */
  void Lua::add(Console* console) {
    if (console == nullptr)
      return;

    engine["console"] = console;
    mConsole          = console;
  }

  /**
   * @brief
   *   This will try to load a filename. If it cannot find the file,
   *   or if the file fails to load due to syntax errors,
   *   it will output an error.
   *
   * @param filename
   *
   * @return
   */
  bool Lua::loadFile(const std::string& filename) {
    // Any errors that are most likely due to not being able to
    // find the file.
    //
    // Output errors to console, if it exists, instead of
    // our error function
    try {
      bool isValid = engine.script_file(filename).valid();

      // if console is active, error to that, otherwise
      // error to stdout
      if (!isValid && mConsole != nullptr)
        mConsole->error("Failed to load file '" + filename + "'. No such file");
      else if (!isValid)
        mLog->error("Failed to load file '{}'. No such file", filename);

      return isValid;
    } catch (const sol::error& e) {
      if (mConsole != nullptr) {
        mConsole->error("Tried to load file '" + filename + "': " + e.what());
      } else {
        mLog->error(e.what());
      }

      return false;
    }
  }

  /**
   * @brief
   *   Adds a handler for a specific event.
   *
   * @param name the name of the event to handle
   * @param e the function that should be called when the event happens.
   */
  void Lua::on(const std::string& eventName, EventHandler&& e) {
    if (!mHandlers.count(eventName))
      mHandlers[eventName] = { e };
    else
      mHandlers[eventName].push_back(e);
  }

  /**
   * @brief
   *   Emits a signal to all attached handlers for a specific event by
   *   calling each function with one parameter, the lua engine.
   *
   * @param eventName
   */
  void Lua::emit(const std::string& eventName) {
    if (!mHandlers.count(eventName))
      return;

    for (auto eventHandler : mHandlers[eventName])
      eventHandler();
  }

  /**
   * @brief
   *   Checks if a type should be included based on several
   *   checks.
   *
   * @param name the name of the variable
   * @param search the search that the user is doing
   *
   * @return
   */
  bool Lua::shouldIncludeType(const std::string& name,
                              const std::string& search) {
    if (name.find("sol.") == 0)
      return false;

    if (search.find("_") != 0 && name.find("_") == 0)
      return false;

    if (search.find(".") != 0 && name.find(".") == 0)
      return false;

    if (search == "")
      return true;

    return name.find(search) != std::string::npos;
  }

  /**
   * @brief
   *   If this is given a table and a search string that contains `:`, it will
   *   recrusively try to find the variables that is valid for the scope.
   *
   *   For example, given a variable `MyTable:MySubTable:MySubSubTable`, it
   *   will return the variables that are available to `MySubSubTable`
   *
   * @param table the table to inspect
   * @param name the scope name
   * @param divider
   *   defaults to ":", but can be overriden to also
   *   work on normal tables with "."
   *
   * @return
   */
  std::vector<TypePair> Lua::getScope(sol::table&        table,
                                      const std::string& name,
                                      const std::string& divider) {
    size_t                pos   = name.find_first_of(divider);
    std::vector<TypePair> types = {};

    // If there's no more scoping to be done, return what we have.
    if (pos == std::string::npos) {
      for (auto a : table) {
        TypePair t = { a.first.get_type() == sol::type::string ?
                         a.first.as<std::string>() :
                         solTypetoStr(a.first.get_type()),
                       solTypetoStr(a.second.get_type()) };

        if (shouldIncludeType(t.first, name))
          types.push_back(t);
      }

      std::sort(types.begin(), types.end(), compare_by_word);
      return types;
    }

    // Scoping continues!
    std::string scopeName = name.substr(0, pos);
    std::string rest      = name.substr(pos + 1);

    sol::object obj = table[scopeName];

    // Tables and userdata has to be handled a bit differently
    switch (obj.get_type()) {
      case sol::type::userdata: {
        sol::table userData = obj;
        sol::table meta     = userData[sol::metatable_key];
        return getScope(meta, rest);
      }
      case sol::type::table: {
        if (divider.find(".") == std::string::npos)
          return {};

        sol::table t = obj;
        return getScope(t, rest, ".");
      }
      default:
        types.push_back({ scopeName, solTypetoStr(obj.get_type()) });
    }

    std::sort(types.begin(), types.end(), compare_by_word);
    return types;
  }

  /**
   * @brief
   *   Returns the variables that matches the name. If name is an empty
   *   string it will return all variables that are available in
   *   the global scope.
   *
   * @param name
   *
   * @return
   */
  std::vector<TypePair> Lua::getTypenames(const std::string name) {
    sol::table t = engine.globals();
    return getScope(t, name, ":.");
  }
}
