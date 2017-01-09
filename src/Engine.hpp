#ifndef ENGINE_HPP
#define ENGINE_HPP

#include <string>
#include <stack>

#include "OpenGLHeaders.hpp"

#include "State/State.hpp"

struct Asset;
class Input;

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

  GLFWwindow* getWindow() const;

  //! Sends the update signal to the current state
  //! (update signal = draw, animations etc)
  void updateState(float deltaTime);

  //! Initializes the engine and creates the first state
  bool initialize(int argc,
                  char* argv[],
                  int isRefresh = State::INIT,
                  int initState = State::MAINMENU);

  //! Sends the input to current state. If current state returns an int
  //! of value [0-1] changes state.
  void keyboardCB(int key, int scan, int action, int mods);

  //! Sends the input to the current state.
  void mouseMovementCB(double x, double y);

  //! Sends the input to the current state. If current state returns an
  //! int of value [0-1] changes state.
  void mouseButtonCB(int button, int action, int mods);

  //! Sends the input to the current state.
  void mouseScrollCB(double offsetx, double offsety);

  void runLoop();

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

  std::stack<int> states;

  std::string cfgPath;

  State *current;
  Input *input;
  GLFWwindow *window;
  Asset *asset;
};

#endif