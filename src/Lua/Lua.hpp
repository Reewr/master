#pragma once

#include <sol.hpp>

namespace sol {
  class state;
}

class Engine;
class CFG;
class Console;

namespace Input {
  class Input;
}

namespace Lua {
  class Lua {
  public:
    Lua();
    ~Lua();

    // TODO: Add removal functions
    void add(Console* c);
    void add(CFG* cfg);

    void reInitialize();
    bool loadFile(const std::string& filename);

    sol::state engine;

  private:
    bool hasConsole = false;
  };
}
