#include "Engine.hpp"

#include "GlobalLog.hpp"
#include "OpenGLHeaders.hpp"

#include "Drawable/Drawable.hpp"
#include "GLSL/Shader.hpp"
#include "GUI/GUI.hpp"
#include "Graphical/Framebuffer.hpp"
#include "Input/Event.hpp"
#include "Input/Input.hpp"
#include "Lua/Lua.hpp"
#include "Resource/ResourceManager.hpp"
#include "State/MainMenu.hpp"
#include "State/Master.hpp"
#include "Utils/Asset.hpp"
#include "Utils/CFG.hpp"
#include "Utils/Utils.hpp"
#include "Utils/str.hpp"

// These functions are because of GLFW's callback. They call engine functions
static void placementKeyboardCB(GLFWwindow* w, int k, int, int a, int m) {
  Engine*      e = static_cast<Engine*>(glfwGetWindowUserPointer(w));
  Input::Event event(e->input(), k, a, m);
  e->sendEvent(event);
}

static void placementMouseMovementCB(GLFWwindow* w, double x, double y) {
  Engine*      e = static_cast<Engine*>(glfwGetWindowUserPointer(w));
  Input::Event event(e->input(), mmm::vec2(x, y));
  e->sendEvent(event);
}

static void placementMouseButtonCB(GLFWwindow* w, int b, int a, int m) {
  Engine* e = static_cast<Engine*>(glfwGetWindowUserPointer(w));

  double x, y;
  glfwGetCursorPos(w, &x, &y);

  Input::Event event(e->input(), mmm::vec2(x, y), b, a, m);
  e->sendEvent(event);
}

static void placementMouseScrollCB(GLFWwindow* w, double ox, double oy) {
  Engine* e = static_cast<Engine*>(glfwGetWindowUserPointer(w));
  double  x, y;
  glfwGetCursorPos(w, &x, &y);
  Input::Event event(e->input(), mmm::vec2(x, y), mmm::vec2(ox, oy));
  e->sendEvent(event);
}

static void placementCharCB(GLFWwindow* w, unsigned int codePoint) {
  Engine*      e = static_cast<Engine*>(glfwGetWindowUserPointer(w));
  Input::Event event(e->input(), str::utf8toStr(codePoint));
  e->sendEvent(event);
}

static void glfwErrorHandler(int, const char* k) {
  error("GLFW_ERROR: {}", k);
}

Engine::Engine(std::string cfgPath)
    : Logging::Log("Engine")
    , mCFGPath(cfgPath)
    , mCFG(nullptr)
    , mCurrent(nullptr)
    , mInput(nullptr)
    , mWindow(nullptr)
    , mAsset(nullptr)
    , mLua(nullptr)
    , mResourceManager(nullptr)
    , mWindowRefresh(false) {}

Engine::~Engine() {
  deinitialize();
}

/**
 * @brief
 *   Returns the input manager
 *
 * @return
 */
Input::Input* Engine::input() {
  return mInput;
}

/**
 * @brief
 *   Initializes the engine by first loading the configuration
 *   file, followed by doing some set of operations depending on
 *   `isRefresh`.
 *
 *   If `isRefresh` is an enum of States::Init, it will assume
 *   that this is the first time this function runs and therefore
 *   initialize all the libraries and classes that are needed.
 *
 *   If `isRefresh` is an enum of States::Refresh, it will
 *   assume that this is not the first time calling this function
 *   and it will only update window size, viewport and swap intervals
 *   together with recreating audio and input classes
 *
 *   Lastly, if `isRefresh` is States::Init, it will also add
 *   a new state to the state stack, effectively creating
 *   a new state of the type given.
 *
 * @param argc
 *   the number of arguments
 *
 * @param argv[]
 *   The argument themselves
 *
 * @param isRefresh
 *   An enum describing the type of initialization
 *
 * @param initState
 *   The state to create, if isRefresh is `States::Init`
 *
 * @return
 *  returns true if everything goes according to the plan,
 *  else false
 */
bool Engine::initialize(int argc, char* argv[], int isRefresh, int initState) {

  mCFG             = new CFG();
  mAsset           = new Asset(mCFG);
  mResourceManager = new ResourceManager();
  // Load the configuration file
  mCFG->assimilate(mCFGPath);

  if (isRefresh == States::Init)
    mCFG->assimilate(argc, argv);

  // DEBUG_MODE    = mCFG->general.debug;
  // ENABLE_COLORS = mCFG->general.debugColors;

  // Init the different libraries if
  // this is the first init call
  if (isRefresh != States::Refresh) {
    if (!initGLFW() || !initWindow() || !initOpenGLBindings())
      return false;
    initGL();

    // Set the callbacks to the different inputs. This has
    // to be done in weird way as glfw is a C library and does
    // not support lambdas with state
    glfwSetKeyCallback(mWindow, placementKeyboardCB);
    glfwSetScrollCallback(mWindow, placementMouseScrollCB);
    glfwSetCursorPosCallback(mWindow, placementMouseMovementCB);
    glfwSetMouseButtonCallback(mWindow, placementMouseButtonCB);
    glfwSetCharCallback(mWindow, placementCharCB);
    glfwSetWindowUserPointer(mWindow, this);
  } else {
    // Else we just readjust the window
    // based on the settings given
    mWindow = glfwGetCurrentContext();
    glfwSetWindowSize(mWindow,
                      (int) mCFG->graphics.res.x,
                      (int) mCFG->graphics.res.y);
    glViewport(0, 0, mCFG->graphics.res.x, mCFG->graphics.res.y);
    glfwSwapInterval(mCFG->graphics.vsync ? 1 : 0);
  }

  mInput = new Input::Input(mWindow, mCFG);

  mLua = new Lua::Lua(mCFG,
                      Lua::Lib::Native::Package | Lua::Lib::Native::Base |
                        Lua::Lib::Native::Table | Lua::Lib::Native::Math |
                        Lua::Lib::Native::String,
                      Lua::Lib::Engine::All);
  mAsset->setInput(mInput);
  mAsset->setLua(mLua);
  mAsset->setResourceManager(mResourceManager);

  mResourceManager->loadDescription("./media/resources.lua");

  Drawable::mAsset = mAsset;
  GUI::mAsset      = mAsset;
  Shader::mCFG     = mCFG;
  /* Spider::init(); */
  /* Model::init(&asset->cfg); */
  Framebuffer::init(mAsset->cfg());

  mLog->debug("Initialized successfully...");

  if (isRefresh == States::Init) {
    changeState(initState);
  }

  return true;
}

/**
 * @brief
 *   Sets the error callback and initializes GLFW.
 *   Returns false if things arent initialized correctly
 *
 * @return false if error otherwise true
 */
bool Engine::initGLFW() {
  glfwSetErrorCallback(glfwErrorHandler);

  if (!glfwInit()) {
    mLog->error("Could not start GLFW - Check GLFW error");
    return false;
  }

  mLog->info("GLFW started successfully");
  return true;
}

/**
 * @brief
 *   Initializes the opengl binding, returning
 *   false and logging any errors.
 *
 * @return true if successful
 */
bool Engine::initOpenGLBindings() {
  if (ogl_LoadFunctions() != ogl_LOAD_SUCCEEDED) {
    mLog->error("Failed to initialize OGL");
    return false;
  }

  // check for opengl errors
  if (!Utils::getGLError())
    return false;

  mLog->info("OGL initialized successfully...");
  return true;
}

/**
 * @brief
 *   This function initializes the GLFW window with the correct
 *   settings based on what it can find in the CFG.
 *
 * @return
 */
bool Engine::initWindow() {
  bool isFullscreen = mCFG->graphics.winMode == 1;

  // Set version we want
  glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
  glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);

  glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);

  // Set the debug context, since we are developing
  glfwWindowHint(GLFW_OPENGL_DEBUG_CONTEXT, GL_TRUE);
  glfwWindowHint(GLFW_SAMPLES, mCFG->graphics.aliasing);

  // No resizing, because of the resolutions in settings
  glfwWindowHint(GLFW_RESIZABLE, GL_FALSE);
  glfwWindowHint(GLFW_FLOATING, isFullscreen ? GL_FALSE : GL_TRUE);

  // If window mode, do decorated
  glfwWindowHint(GLFW_DECORATED, isFullscreen ? GL_FALSE : GL_TRUE);

  // get monitor, may be null, but thats okay since glfw supports it
  GLFWmonitor* monitor = getMonitor();
  mWindow              = glfwCreateWindow((int) mCFG->graphics.res.x,
                             (int) mCFG->graphics.res.y,
                             "Wooooo",
                             monitor,
                             NULL);

  if (!mWindow) {
    mLog->error("Could not open window with GLFW. Check GLFW error messages");
    glfwTerminate();
    return false;
  }

  // Make the window our openGL context
  glfwMakeContextCurrent(mWindow);

  // Set vsync if set by config
  glfwSwapInterval(mCFG->graphics.vsync ? 1 : 0);

  mLog->info("GLFW Window settings initalized successfully...");
  return true;
}

/**
 * @brief
 *   Initialize OpenGL with the settings that we need.
 */
void Engine::initGL() {
  glViewport(0, 0, mCFG->graphics.res.x, mCFG->graphics.res.y);

  glEnable(GL_BLEND);
  glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

  // useful for debug, draw wire-frame edges
  // glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);

  glEnable(GL_DEPTH_TEST);
  glDepthFunc(GL_LESS);

  glEnable(GL_CULL_FACE);
  glCullFace(GL_BACK);
  glFrontFace(GL_CCW);

  if (mCFG->graphics.aliasing != 0)
    glEnable(GL_MULTISAMPLE);
}

/**
 * @brief
 *   Retrieve the correct monitor based on the configuration give through the
 *   config file. For simplicity, which monitor to use is only considered if
 *   the user uses Fullscreen mode. It will choose the monitor that the user
 *   has selected as long as it is valid, otherwise we use the primary monitor.
 *
 *   When using fullscreen mode we will also make sure that we get the correct
 *   resolution of the monitor
 *
 * @return
 *   Null if fullscreen isnt enabled otherwise a monitor
 */
GLFWmonitor* Engine::getMonitor() {
  int                monitorCount;
  GLFWmonitor*       monitor   = NULL;
  const GLFWvidmode* videoMode = NULL;
  GLFWmonitor**      monitors  = glfwGetMonitors(&monitorCount);

  // if we are running fullscreen, we gotta use the correct monitor
  if (mCFG->graphics.winMode == 2) {

    // if the user has selected a monitor and its
    // below the max number of monitors we use it,
    // otherwise use primary
    if (mCFG->graphics.monitor < monitorCount)
      monitor = monitors[mCFG->graphics.monitor];
    else
      monitor = glfwGetPrimaryMonitor();

    // since its fullscreen, we get the resolution of the monitor
    videoMode          = glfwGetVideoMode(monitor);
    mCFG->graphics.res = mmm::vec2(videoMode->width, videoMode->height);
  }

  return monitor;
}

void Engine::sendEvent(const Input::Event& event) {
  if (mCurrent == nullptr)
    return mLog->error("Current state is null");

  mCurrent->input(event);

  if (event.state() != States::NoChange) {
    changeState(event.state());
  }
}

/**
 * @brief
 *   Deinitializes the engine, deleting all the resources and calling other
 *   deinit functions. Also terminates glfw if `isFullDeinit` is true.
 *
 * @param isFullDeinit
 *   If true, also shuts down glfw.
 */
void Engine::deinitialize(bool isFullDeinit) {
  if (mCurrent != nullptr) {
    delete mCurrent;
    mCurrent = nullptr;
  }

  mResourceManager->unloadAll();

  mCFG->writetoFile("config/config.ini");

  if (mAsset != nullptr) {
    delete mAsset;
    mAsset = nullptr;
  }

  if (mInput != nullptr) {
    delete mInput;
    mInput = nullptr;
  }

  if (mCFG != nullptr) {
    delete mCFG;
    mCFG = nullptr;
  }

  if (mLua != nullptr) {
    delete mLua;
    mLua = nullptr;
  }

  if (mResourceManager != nullptr) {
    delete mResourceManager;
    mResourceManager = nullptr;
  }

  if (isFullDeinit) {
    glfwDestroyWindow(mWindow);
    glfwTerminate();
    mWindow = nullptr;
  }
}

/**
 * @brief
 *   This function runs the actual main loop. It will also calculate the delta
 *   time. basically, how long the previous loop spent doing its things. If
 *   vsync is on, this will most likely be a static ~17.6ms.
 *
 *   Calls updateState(deltaTime) and also polls for event
 */
void Engine::runLoop() {
  // Used to check the difference between the loops
  static float startTime = glfwGetTime();

  // Lock the frames to 30 FPS.
  // Set this to a high number or comment out the
  // remaining loop time if you want more frames.
  //
  // The reason for this lock is so that the movement of the
  // character is slower and therefore more visible to the human
  // eye. This is purely for display and presentation purposes and
  // will eventually be removed
  static int   numFrames = 30;
  static float loopTime  = 1.0 / float(numFrames);

  while (!glfwWindowShouldClose(mWindow)) {
    float currentTime = glfwGetTime();
    float deltaTime   = currentTime - startTime;
    startTime         = currentTime;

    // Update the stack if available
    mCurrent->update(deltaTime);

    // Clear everything
    // glClearColor(0, 0.4, 0.7, 1);
    // glClearColor(0.15, 0.15, 0.18, 1);
    // White color is used to the background seamlessly matches the color of
    // the paper.
    glClearColor(1.0, 1.0, 1.0, 1.0);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    mCurrent->draw(deltaTime);

    glfwSwapBuffers(mWindow);
    glfwPollEvents();

    float remainingLoopTime = loopTime - (glfwGetTime() - currentTime);

    // Lock FPS to specific duration. See comment before loop started
    if (remainingLoopTime > 0) {
      std::chrono::milliseconds ms(int(remainingLoopTime * 1000));
      std::this_thread::sleep_for(ms);
    }

    // Special case for when the engine has to be entirely
    // reloaded due context setting changes.
    //
    // The reason for this so late in the loop is
    // so that no GLFW calls are being called during
    // its destruction
    if (mWindowRefresh) {
      refreshState(States::WinRefresh);
      int current = mActiveStates.top();
      mActiveStates.pop();
      changeState(current);
      mWindowRefresh = false;
    }
  }
}

/**
 * @brief
 *   Refreshes the state by checking the new configuration that might have been
 *   added and sets those.
 *
 * @param refreshType
 *   The type of refresh. If it is equal to States::WinRefresh, it will
 *   deinitialize the entire engine before reinitializing it again.
 */
void Engine::refreshState(int refreshType) {
  deinitialize(refreshType == States::WinRefresh);

  if (!initialize(0, NULL, refreshType)) {
    throw std::runtime_error("Engine failed to refresh!");
  }
}

/**
 * @brief
 *   Changes the state to a new state specified by `newState`.
 *
 * @param newState
 */
void Engine::changeState(int newState) {
  switch (newState) {
    case States::QuitAll:
      while (!mActiveStates.empty())
        mActiveStates.pop();
      break;
    case States::Quit:
      mActiveStates.pop();
      break;
    // Fall through because they use the same function
    case States::Refresh:
      refreshState(newState);
      break;
    case States::WinRefresh:
      mWindowRefresh = true;
      return;
    case States::LuaReload:
      mLua->reInitialize();
      return;
    default:
      mActiveStates.push(newState);
      break;
  }

  if (mActiveStates.size() == 0)
    return closeWindow();

  if (mCurrent != nullptr) {
    delete mCurrent;
    mCurrent = nullptr;
  }

  switch (mActiveStates.top()) {
    case States::MainMenu:
      mCurrent = new MainMenu(mAsset);
      break;
    case States::Game:
      throw std::runtime_error("Tried to make game. THROW FIT (╯°□°）╯︵ ┻━┻)");
    case States::MasterThesis:
      mCurrent = new Master(mAsset);
      break;
    default:
      throw std::runtime_error("No state! THROW FIT. (╯°□°）╯︵ ┻━┻)");
  }
}

/**
 * @brief
 *   Tells glfw to close the window. Since the loop checks for a flag which is
 *   called `windowShouldClose`, calling this function effectively ends the
 *   main loop and therefore stops the program
 */
void Engine::closeWindow() {
  mLog->info("Now closing GLFW window..");
  glfwSetWindowShouldClose(mWindow, GL_TRUE);
}
