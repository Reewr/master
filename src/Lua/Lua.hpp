#pragma once

#include <sol.hpp>

class Engine;
class CFG;
class Console;

namespace Input {
  class Input;
}

namespace Lua {

  std::string solTypetoStr(sol::type t);

  struct Typenames {
    std::string name;
    std::string typeName;

    bool operator==(const Typenames& rhs) const;
  };

  class Lua {
  public:
    Lua();
    ~Lua();

    // TODO: Add removal functions
    void add(Console* c);
    void add(CFG* cfg);

    void reInitialize();
    bool loadFile(const std::string& filename);
    std::vector<Typenames> getTypenames(const std::string name="");

    sol::state engine;

  private:
    bool shouldIncludeType(const std::string& name, const std::string& search);
    Typenames getTypename(std::pair<sol::basic_object<sol::reference>,
                                    sol::basic_object<sol::reference>>& a);
    std::vector<Typenames> getScope(sol::table& t, const std::string& name);
    Console* mConsole;
    CFG*     mCFG;
  };
}
