#pragma once

#include <stack>
#include <string>

#include "OpenGLHeaders.hpp"

#include "State/State.hpp"

class Asset;
class CFG;
class ResourceManager;

namespace Input {
  class Input;
  class Event;
}

namespace Lua {
  class Lua;
}

//! Engine sorts out which state that is active
class Engine {
public:
  /**
   * @brief
   *   Creates the engine itself. Running this
   *   function will not do much other than
   *   load the configuration file. The next logical
   *   step is to run the initialize function
   *
   * @param cfgPath
   *   This argument defaults to config/config.ini and
   *   the path is relative to the executable. This is
   *   where it expects to find the configuration file
   */
  Engine(std::string cfgPath = "config/config.ini");

  //! Desctructor that deletes all the allocated objects
  ~Engine();

  //! Sends the update signal to the current state
  //! (update signal = draw, animations etc)
  void updateState(float deltaTime);

  //! Initializes the engine and creates the first state
  bool initialize(int   argc,
                  char* argv[],
                  int   isRefresh = States::Init,
                  int   initState = States::MainMenu);

  void sendEvent(const Input::Event& event);

  void runLoop();

  Input::Input* input();

protected:
  //! Initializesers of the different libraries that are being used
  bool initGLFW();
  bool initOpenGLBindings();
  bool initWindow();
  void initGL();

  GLFWmonitor* getMonitor();

  void deinitialize(bool isFullDeinit = true);

  void refreshState(bool isWinRefresh);
  void changeState(int newState);
  void createState();
  void closeWindow();

  std::stack<int> mActiveStates;

  std::string mCFGPath;

  CFG*          mCFG;
  State*        mCurrent;
  Input::Input* mInput;
  GLFWwindow*   mWindow;
  Asset*        mAsset;
  Lua::Lua*     mLua;
  ResourceManager* mResourceManager;
};
