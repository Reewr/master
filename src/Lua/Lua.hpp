#pragma once

#include "../Log.hpp"
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
  typedef std::pair<std::string, std::string> TypePair;
  // Turns a sol::type to a string of max 6 characters
  std::string solTypetoStr(sol::type t);

  // This namespace describes the libraries that can
  // be loaded with the Lua engine
  namespace Lib {

    // This enum describes the Lua native libraries
    // that can loaded
    enum class Native : int {
      Base      = 1,
      Package   = 2,
      Coroutine = 4,
      String    = 8,
      OS        = 16,
      Math      = 32,
      Table     = 64,
      Debug     = 128,
      Bit32     = 256,
      IO        = 512,
      FFI       = 1024,
      JIT       = 2048,
      UTF8      = 4096,
      Count     = 8192,
      All       = 16383
    };

    // This enum describes the libraries made by
    // this engine that can be loaded
    enum class Engine : int {
      Console  = 1,
      Math     = 2,
      Shape    = 4,
      GUI      = 8,
      Input    = 16,
      CFG      = 32,
      State    = 64,
      Drawable = 128,
      Util     = 256,
      Learning = 512,
      All      = 1023
    };
  }

  class Lua : public Logging::Log {
  public:
    // Creates a Lua instance that holds the Lua State.
    // This allows you to load files that will have C++ object representations
    // in them
    Lua(CFG* cfg, Lib::Native n, Lib::Engine e);
    ~Lua();

    // TODO: Add removal functions

    //! Adds the console object to Lua, making it available to Lua runtime
    void add(Console* c);

    // Loads one or more native libraries based on active bit fields
    void openLibraries(Lib::Native nativeLib);

    // Loads one more more engine libraries based on active bit fields
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
    std::vector<TypePair> getTypenames(const std::string name = "");

    sol::state engine;

  private:
    // Checks the scope, this function will work recurisvely.
    std::vector<TypePair> getScope(sol::table&        t,
                                   const std::string& name,
                                   const std::string& divider = ":");

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
