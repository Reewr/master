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

    //! Adds the console object to Lua, making it available to Lua runtime
    void add(Console* c);

    //! Adds teh CFG object to Lua, making it available to Lua runtime.
    void add(CFG* cfg);

    //! Recreates the lua engine, adding the console and config
    //! objects and reruns the files that are loaded
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
