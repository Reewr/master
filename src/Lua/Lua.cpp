#include "Lua.hpp"

#include "../Console/Console.hpp"
#include "../Engine.hpp"
#include "../Utils/CFG.hpp"
#include "../Utils/Utils.hpp"

#include "Console/LConsole.hpp"
#include "GUI/LText.hpp"
#include "GUI/LMenu.hpp"
#include "GUI/LDropdown.hpp"
#include "GUI/LSlider.hpp"
#include "Input/LEvent.hpp"
#include "LEngine.hpp"
#include "Math/LMath.hpp"
#include "Resource/LFont.hpp"
#include "State/LState.hpp"
#include "Utils/LCFG.hpp"

#include <sol.hpp>

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
    switch(t) {
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
  Lua::Lua() : mConsole(nullptr), mCFG(nullptr) { reInitialize(); }

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
    engine.open_libraries(sol::lib::base, sol::lib::package);

    // Load math
    math_as_lua(engine);

    // Load resources
    font_as_lua(engine);

    // Load GUI
    text_as_lua(engine);
    menu_as_lua(engine);
    dropdown_as_lua(engine);
    slider_as_lua(engine);

    // Load event
    event_as_lua(engine);

    // Load console
    console_as_lua(engine);

    // Load CFG
    cfg_as_lua(engine);

    // Load State
    state_as_lua(engine);

    std::string path          = engine["package"]["path"];
    std::string sep           = path.empty() ? "" : ";";
    engine["package"]["path"] = path + sep + "./lua/?.lua";

    engine.create_named_table("lua");

    engine["lua"]["loadFile"] = [&](const std::string& filename) {
      loadFile(filename);
    };

    if (mConsole != nullptr)
      add(mConsole);
    if (mCFG != nullptr)
      add(mCFG);

    loadFile("lua/main.lua");
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
   *   Tells the Lua engine to add the cfg to global scope
   *   as a `cfg` variable, making it available to Lua code.
   *
   *   It will store the pointer to the cfg so that when
   *   `reinitialize` is done, it can include it again.
   *
   * @param cfg
   */
  void Lua::add(CFG* cfg) {
    if (cfg == nullptr)
      return;

    engine["cfg"] = cfg;
    mCFG          = cfg;
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
      return {pair.first.as<std::string>(), solTypetoStr(secondType)};
    }

    return {solTypetoStr(firstType), solTypetoStr(secondType)};
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
  std::vector<Typenames> Lua::getScope(sol::table& table, const std::string& name) {
    size_t pos = name.find_first_of(":");
    std::vector<Typenames> types = {};

    if (pos == std::string::npos) {
      for (auto a : table) {
        Typenames t = getTypename(a);

        if (shouldIncludeType(t.name, name))
          types.push_back(getTypename(a));
      }

      return types;
    }

    std::string scopeName = name.substr(0, pos);
    std::string rest = name.substr(pos + 1);

    sol::table newTable = engine[scopeName];

    if (newTable)
      return getScope(newTable, rest);

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
    size_t pos = name.find_first_of(":");
    std::vector<Typenames> types = {};

    // No scoping, just retrieve the variables
    // that match name in global scope
    if (pos == std::string::npos) {
      for(auto a : engine) {
        Typenames t = getTypename(a);

        if (shouldIncludeType(t.name, name))
          types.push_back(getTypename(a));
      }

      std::sort(types.begin(), types.end(), compare_by_word);
      return types;
    }

    // There's scoping to be done. Find the variables that are
    // valid under that scope.
    std::string scopeName = name.substr(0, pos);
    std::string rest = name.substr(pos + 1);

    sol::object obj = engine[scopeName];

    switch(obj.get_type()) {

      // Userdata has a special handler since in order to retrieve the variables
      // you have to check the metatable instead of iterating over keys
      case sol::type::userdata: {
        sol::table userData = obj;
        sol::table meta     = userData[sol::metatable_key];

        for (auto pair : meta) {
          std::string key = pair.first.as<std::string>();

          if (shouldIncludeType(key, rest))
            types.push_back({key, solTypetoStr(pair.second.get_type())});

        }
        break;
      }

      // Tables are easy to retrieve, since you can just keep
      // iterating over key/values
      case sol::type::table: {
        sol::table table = obj;

        for(auto pair : table) {
          std::string key = pair.first.as<std::string>();

          if (shouldIncludeType(key, rest))
            types.push_back({key, solTypetoStr(pair.second.get_type())});
        }
        break;
      }
      default:
        types.push_back({scopeName, solTypetoStr(obj.get_type())});
    }

    std::sort(types.begin(), types.end(), compare_by_word);
    return types;
  }

}
