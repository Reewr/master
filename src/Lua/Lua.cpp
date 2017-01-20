#include "Lua.hpp"

#include "../Engine.hpp"
#include "../Utils/CFG.hpp"
#include "../Console/Console.hpp"
#include "../Utils/Utils.hpp"

#include "Math.hpp"
#include "Font.hpp"
#include "Text.hpp"
#include "Event.hpp"
#include "Engine.hpp"
#include "Console.hpp"
#include "CFG.hpp"
#include "State.hpp"

#include <sol.hpp>

namespace Lua {
  Lua::Lua() {
    reInitialize();
  }

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

    std::string path = engine["package"]["path"];
    std::string sep  = path.empty() ? "" : ";";
    engine["package"]["path"] = path + sep + "./lua/?.lua";

    engine.create_named_table("lua");

    engine["lua"]["loadFile"] = [&](const std::string& filename) {
      loadFile(filename);
    };

    engine["lua"]["reload"] = [&]() {
      reInitialize();
    };

    loadFile("lua/main.lua");
  }

  bool Lua::loadFile(const std::string& filename) {
    try {
      sol::function_result res = engine.script_file("lua/main.lua");
      return res.valid();

    } catch (const sol::error& e) {
      if (hasConsole) {
        Console* c = engine["console"];
        c->error("Tried to load file '" + filename + "': " + e.what());
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
    hasConsole = true;
  }

  void Lua::add(CFG* cfg) {
    if (cfg == nullptr)
      return;

    engine["cfg"] = cfg;
  }
}
