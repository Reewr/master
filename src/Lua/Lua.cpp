#include "Lua.hpp"

#include "../Console/Console.hpp"
#include "../Engine.hpp"
#include "../Utils/CFG.hpp"
#include "../Utils/Utils.hpp"

#include "Utils/CFG.hpp"
#include "Console/Console.hpp"
#include "Engine.hpp"
#include "Input/Event.hpp"
#include "Resource/Font.hpp"
#include "Math/Math.hpp"
#include "State/State.hpp"
#include "GUI/Text.hpp"

#include <sol.hpp>

namespace Lua {
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
