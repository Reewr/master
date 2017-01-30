#pragma once

#include <sol.hpp>
#include <map>
#include <vector>
#include <functional>

class Engine;
class CFG;
class Console;

namespace Input {
  class Input;
}

namespace Lua {


  typedef std::function<void()> EventHandler;
  // Turns a sol::type to a string of max 6 characters
  std::string solTypetoStr(sol::type t);

  // This is a simple struct used to make it easier to
  // store the variables available in lua
  struct Typenames {
    std::string name;
    std::string typeName;

    // equal operator
    bool operator==(const Typenames& rhs) const;
  };

  class Lua {
  public:

    // Creates a Lua instance that holds the Lua State.
    // This allows you to load files that will have C++ object representations
    // in them
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

    // loads a file by filename/path. This is according to where
    // the working directory is.
    bool loadFile(const std::string& filename);

    // Sets a handler to handle a specific event
    void on(const std::string& eventName, EventHandler&& e);

    // Calls all handlers for a specific event
    void emit(const std::string& eventName);

    // Returns a list of types that is accessible based on the name given.
    // For instance, a name with "" will check global scope where as
    // a name with "config:" will check the scope of "config"
    std::vector<Typenames> getTypenames(const std::string name="");

    sol::state engine;

  private:

    // Simple way of creating a Typenames struct based on a pair given
    // by a table.
    Typenames getTypename(std::pair<sol::basic_object<sol::reference>,
                                    sol::basic_object<sol::reference>>& a);

    // Checks the scope, this function will work recurisvely.
    std::vector<Typenames> getScope(sol::table& t, const std::string& name);
    // Function used by both getTypenames and getScope to see if they should
    // ignore a variable based on what the name contains / does not contain
    bool shouldIncludeType(const std::string& name, const std::string& search);

    std::map<std::string, std::vector<EventHandler>> mHandlers;
    Console* mConsole;
    CFG*     mCFG;
  };
}
