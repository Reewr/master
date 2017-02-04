#pragma once

#include <functional>
#include <map>
#include <sol.hpp>
#include <vector>

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

  // This namespace describes the libraries that can
  // be loaded with the Lua engine
  namespace Lib {

    // This enum describes the Lua native libraries
    // that can loaded
    enum class Native : int {
      Base      = 0x01,
      Package   = 0x02,
      Coroutine = 0x04,
      String    = 0x08,
      OS        = 0x16,
      Math      = 0x32,
      Table     = 0x64,
      Debug     = 0x128,
      Bit32     = 0x256,
      IO        = 0x512,
      FFI       = 0x1024,
      JIT       = 0x2048,
      UTF8      = 0x4096,
      Count     = 0x8192,
      All       = 0x16384
    };

    // This enum describes the libraries made by
    // this engine that can be loaded
    enum class Engine : int {
      Console = 0x01,
      Math    = 0x02,
      Shape   = 0x04,
      GUI     = 0x08,
      Input   = 0x16,
      CFG     = 0x32,
      State   = 0x064,
      Util    = 0x128,
      All     = 0x255
    };
  }

  class Lua {
  public:
    // Creates a Lua instance that holds the Lua State.
    // This allows you to load files that will have C++ object representations
    // in them
    Lua(CFG* cfg, Lib::Native n, Lib::Engine e);
    ~Lua();

    // TODO: Add removal functions

    //! Adds the console object to Lua, making it available to Lua runtime
    void add(Console* c);

    // Loads one or more native libraries
    void openLibraries(Lib::Native nativeLib);

    // Loads one more more engine libraries
    void openLibraries(Lib::Engine engine);

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
    std::vector<Typenames> getTypenames(const std::string name = "");

    sol::state engine;

  private:
    // Simple way of creating a Typenames struct based on a pair given
    // by a table.
    Typenames getTypename(std::pair<sol::basic_object<sol::reference>,
                                    sol::basic_object<sol::reference>>& a);

    // Checks the scope, this function will work recurisvely.
    std::vector<Typenames> getScope(sol::table& t,
                                    const std::string& name,
                                    const std::string& divider=":");
    // Function used by both getTypenames and getScope to see if they should
    // ignore a variable based on what the name contains / does not contain
    bool shouldIncludeType(const std::string& name, const std::string& search);

    std::map<std::string, std::vector<EventHandler>> mHandlers;
    Console* mConsole;
    CFG*     mCFG;

    Lib::Native mNativeLibraries;
    Lib::Engine mEngineLibraries;
  };
}

// Overloading for the Lib so that bitwise operations work properly
constexpr Lua::Lib::Native operator|(Lua::Lib::Native a, Lua::Lib::Native b) {
  return static_cast<Lua::Lib::Native>(static_cast<int>(a) |
                                       static_cast<int>(b));
}

constexpr Lua::Lib::Native operator&(Lua::Lib::Native a, Lua::Lib::Native b) {
  return static_cast<Lua::Lib::Native>(static_cast<int>(a) &
                                       static_cast<int>(b));
}

constexpr Lua::Lib::Engine operator|(Lua::Lib::Engine a, Lua::Lib::Engine b) {
  return static_cast<Lua::Lib::Engine>(static_cast<int>(a) |
                                       static_cast<int>(b));
}

constexpr Lua::Lib::Engine operator&(Lua::Lib::Engine a, Lua::Lib::Engine b) {
  return static_cast<Lua::Lib::Engine>(static_cast<int>(a) &
                                       static_cast<int>(b));
}
