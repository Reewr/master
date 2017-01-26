#include "Lua.hpp"

#include "../Console/Console.hpp"
#include "../Engine.hpp"
#include "../Utils/CFG.hpp"
#include "../Utils/Utils.hpp"

#include "Console/LConsole.hpp"
#include "GUI/LText.hpp"
#include "Input/LEvent.hpp"
#include "LEngine.hpp"
#include "Math/LMath.hpp"
#include "Resource/LFont.hpp"
#include "State/LState.hpp"
#include "Utils/LCFG.hpp"

#include <sol.hpp>

bool compare_by_word(const Lua::Typenames& t1, const Lua::Typenames t2) {
  return t1.name < t2.name;
}

namespace Lua {

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

  bool Typenames::operator==(const Typenames& rhs) const {
    return name == rhs.name && typeName == rhs.typeName;
  }

  Lua::Lua() : mConsole(nullptr), mCFG(nullptr) { reInitialize(); }

  Lua::~Lua() {}

  void Lua::reInitialize() {
    engine = sol::state();
    engine.open_libraries(sol::lib::base, sol::lib::package);

    math_as_lua(engine);
    font_as_lua(engine);
    text_as_lua(engine);
    event_as_lua(engine);
    console_as_lua(engine);
    engine_as_lua(engine);
    cfg_as_lua(engine);
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

  bool Lua::loadFile(const std::string& filename) {
    try {
      sol::function_result res     = engine.script_file("lua/main.lua");
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

  std::vector<Typenames> Lua::getScope(sol::table& t, const std::string& name) {
    size_t pos = name.find_first_of(":");
    std::vector<Typenames> types = {};

    if (pos == std::string::npos) {
      for (auto a : t) {
        Typenames t = getTypename(a);

        if (shouldIncludeType(t.name, name))
          types.push_back(getTypename(a));
      }

      return types;
    }

    std::string scopeName = name.substr(0, pos);
    std::string rest = name.substr(pos + 1);

    sol::table table = engine[scopeName];

    if (table)
      return getScope(table, rest);

    return types;
  }

  std::vector<Typenames> Lua::getTypenames(const std::string name) {
    size_t pos = name.find_first_of(":");
    std::vector<Typenames> types = {};

    if (pos == std::string::npos) {
      for(auto a : engine) {
        Typenames t = getTypename(a);

        if (shouldIncludeType(t.name, name))
          types.push_back(getTypename(a));
      }

      std::sort(types.begin(), types.end(), compare_by_word);
      return types;
    }

    std::string scopeName = name.substr(0, pos);
    std::string rest = name.substr(pos + 1);

    sol::object obj = engine[scopeName];

    switch(obj.get_type()) {
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

  void Lua::add(Console* console) {
    if (console == nullptr)
      return;

    engine["console"] = console;
    mConsole          = console;
  }

  void Lua::add(CFG* cfg) {
    if (cfg == nullptr)
      return;

    engine["cfg"] = cfg;
    mCFG          = cfg;
  }
}
