#include "Lua.hpp"

#include "../Console/Console.hpp"
#include "../Engine.hpp"
#include "../Utils/CFG.hpp"
#include "../Utils/Utils.hpp"

#include <sol.hpp>

#include "LuaLib.hpp"

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
bool compare_by_word(const Lua::Typenames& t1, const Lua::Typenames t2) {
  return t1.name < t2.name;
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
   *   Equal operator for Typenames
   *
   * @param rhs
   *
   * @return
   */
  bool Typenames::operator==(const Typenames& rhs) const {
    return name == rhs.name && typeName == rhs.typeName;
  }

  /**
   * @brief
   *   Initializes the Lua class, creating the Lua State and exporting
   *   several C++ objects to Lua.
   */
  Lua::Lua(CFG* c) : mConsole(nullptr), mCFG(c) { reInitialize(); }

  Lua::~Lua() {}

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
    engine.open_libraries(sol::lib::base, sol::lib::table, sol::lib::package);
    engine.require("Console",
                   sol::c_call<decltype(&LuaLib::openConsole),
                               &LuaLib::openConsole>,
                   false);

    engine.require("Math",
                   sol::c_call<decltype(&LuaLib::Math::openMath),
                               &LuaLib::Math::openMath>,
                   false);
    // Load math
    engine.require("Math.vec2",
                   sol::c_call<decltype(&LuaLib::Math::openVec2),
                               &LuaLib::Math::openVec2>,
                   false);

    // Shapes
    engine.require("Shape",
                   sol::c_call<decltype(&LuaLib::Shape::openShape),
                               &LuaLib::Shape::openShape>,
                   false);
    engine.require("Shape.Rectangle",
                   sol::c_call<decltype(&LuaLib::Shape::openRectangle),
                               &LuaLib::Shape::openRectangle>,
                   false);

    // Load GUI
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

    // Load event
    engine.require("Input",
                   sol::c_call<decltype(&LuaLib::Input::openInput),
                               &LuaLib::Input::openInput>,
                   false);
    engine.require("Input.Event",
                   sol::c_call<decltype(&LuaLib::Input::openEvent),
                               &LuaLib::Input::openEvent>,
                   false);

    engine.require("Input.Event.Type",
                   sol::c_call<decltype(&LuaLib::Input::openEventType),
                               &LuaLib::Input::openEventType>,
                   false);
    // Load CFG
    engine.require("CFG",
                   sol::c_call<decltype(&LuaLib::openCFG),
                               &LuaLib::openCFG>,
                   false);
    // Load State
    engine.require("State",
                   sol::c_call<decltype(&LuaLib::State::openState),
                               &LuaLib::State::openState>,
                   false);
    engine.require("State.Type",
                   sol::c_call<decltype(&LuaLib::State::openStateTypes),
                               &LuaLib::State::openStateTypes>,
                   false);

    engine.require("Util",
                   sol::c_call<decltype(&LuaLib::Util::openUtil),
                               &LuaLib::Util::openUtil>,
                   false);

    std::string path          = engine["package"]["path"];
    std::string sep           = path.empty() ? "" : ";";
    engine["package"]["path"] = path + sep + "./lua/?.lua";

    engine.create_named_table("lua");

    engine["cfg"] = mCFG;
    engine["lua"]["loadFile"] = [&](const std::string& filename) {
      loadFile(filename);
    };

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
      sol::function_result res     = engine.script_file(filename);
      bool                 isValid = res.valid();

      // if console is active, error to that, otherwise
      // error to stdout
      if (!isValid && mConsole != nullptr)
        mConsole->error("Failed to load file '" + filename + "'. No such file");
      else if (!isValid)
        error("Failed to load file '" + filename + "'. No such file");

      return isValid;
    } catch (const sol::error& e) {
      if (mConsole != nullptr) {
        mConsole->error("Tried to load file '" + filename + "': " + e.what());
      } else {
        error(e.what());
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
   *   Turns a pair, retrieved from iterating over tables/states,
   *   into a Typenames structure.
   *
   * @param pair
   *
   * @return
   */
  Typenames
  Lua::getTypename(std::pair<sol::basic_object<sol::reference>,
                             sol::basic_object<sol::reference>>& pair) {
    sol::type firstType  = pair.first.get_type();
    sol::type secondType = pair.second.get_type();

    if (firstType == sol::type::string) {
      return { pair.first.as<std::string>(), solTypetoStr(secondType) };
    }

    return { solTypetoStr(firstType), solTypetoStr(secondType) };
  }

  /**
   * @brief
   *   If this is given a table and a search string that contains `:`, it will
   *   recrusively try to find the variables that is valid for the scope.
   *
   *   For example, given a variable `MyTable:MySubTable:MySubSubTable`, it
   *   will return the variables that are available to `MySubSubTable`
   *
   * @param t
   * @param name
   *
   * @return
   */
  std::vector<Typenames> Lua::getScope(sol::table&        table,
                                       const std::string& name) {
    size_t                 pos   = name.find_first_of(":");
    std::vector<Typenames> types = {};

    if (pos == std::string::npos) {
      for (auto a : table) {
        Typenames t = getTypename(a);

        if (shouldIncludeType(t.name, name))
          types.push_back(getTypename(a));
      }

      std::sort(types.begin(), types.end(), compare_by_word);
      return types;
    }

    std::string scopeName = name.substr(0, pos);
    std::string rest      = name.substr(pos + 1);
    log("ScopeName: ", scopeName, ", rest: ", rest);

    sol::object obj = table[scopeName];

    switch (obj.get_type()) {
      case sol::type::userdata: {
        sol::table userData = obj;
        sol::table meta     = userData[sol::metatable_key];
        return getScope(meta, rest);
      }
      case sol::type::table: {
        sol::table t = obj;
        return getScope(t, rest);
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
  std::vector<Typenames> Lua::getTypenames(const std::string name) {
    sol::table t = engine.globals();
    return getScope(t, name);
  }
}
