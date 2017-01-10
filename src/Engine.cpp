#include "Engine.hpp"

#include "OpenGLHeaders.hpp"

#include "GUI/GUI.hpp"
#include "Graphical/Font.hpp"
#include "Graphical/Framebuffer.hpp"
#include "Graphical/Text.hpp"
#include "Graphical/Texture.hpp"
#include "Utils/Asset.hpp"
#include "Utils/Utils.hpp"
#include "State/MainMenu.hpp"
#include "State/Master.hpp"
/* #include <3D/Spider.hpp> */
/* #include <3D/Model.hpp> */
#include "Input.hpp"

// These functions are because of GLFW's callback. They call engine functions
static void placementKeyboardCB(GLFWwindow *w, int k, int s, int a, int m) {
  Engine *e = static_cast<Engine *>(glfwGetWindowUserPointer(w));
  e->keyboardCB(k, s, a, m);
}

static void placementMouseMovementCB(GLFWwindow *w, double x, double y) {
  Engine *e = static_cast<Engine *>(glfwGetWindowUserPointer(w));
  e->mouseMovementCB(x, y);
}

static void placementMouseButtonCB(GLFWwindow *w, int b, int a, int m) {
  Engine *e = static_cast<Engine *>(glfwGetWindowUserPointer(w));
  e->mouseButtonCB(b, a, m);
}

static void placementMouseScrollCB(GLFWwindow *w, double ox, double oy) {
  Engine *e = static_cast<Engine *>(glfwGetWindowUserPointer(w));
  e->mouseScrollCB(ox, oy);
}

static void glfwErrorHandler(int, const char *k) { error("GLFW_ERROR: ", k); }

Engine::Engine(std::string cfgPath) { this->cfgPath = cfgPath; }

Engine::~Engine() { deinitialize(); }

/**
 * @brief
 *   Initializes the engine by first loading the configuration
 *   file, followed by doing some set of operations depending on
 *   `isRefresh`.
 *
 *   If `isRefresh` is an enum of State::INIT, it will assume
 *   that this is the first time this function runs and therefore
 *   initialize all the libraries and classes that are needed.
 *
 *   If `isRefresh` is an enum of State::Refresh, it will
 *   assume that this is not the first time calling this function
 *   and it will only update window size, viewport and swap intervals
 *   together with recreating audio and input classes
 *
 *   Lastly, if `isRefresh` is State::INIT, it will also add
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
 *   The state to create, if isRefresh is `State::INIT`
 *
 * @return
 *  returns true if everything goes according to the plan,
 *  else false
 */
bool Engine::initialize(int argc, char *argv[], int isRefresh, int initState) {
  asset = new Asset();

  // Load the configuration file
  asset->cfg.assimilate(cfgPath.c_str());
  if (isRefresh == State::INIT)
    asset->cfg.assimilate(argc, argv);

  // Init the different libraries if
  // this is the first init call
  if (isRefresh != State::REFRESH) {
    if (!initGLFW() || !initWindow() || !initOpenGLBindings())
      return false;
    initGL();

    // Set the callbacks to the different inputs. This has
    // to be done in weird way as glfw is a C library and does
    // not support lambdas with state
    glfwSetWindowUserPointer(window, this);
    glfwSetKeyCallback(window, placementKeyboardCB);
    glfwSetScrollCallback(window, placementMouseScrollCB);
    glfwSetCursorPosCallback(window, placementMouseMovementCB);
    glfwSetMouseButtonCallback(window, placementMouseButtonCB);
  } else {
    // Else we just readjust the window
    // based on the settings given
    window = glfwGetCurrentContext();
    glfwSetWindowSize(window, (int)asset->cfg.graphics.res.x,
                      (int)asset->cfg.graphics.res.y);
    glViewport(0, 0, asset->cfg.graphics.res.x, asset->cfg.graphics.res.y);
    glfwSwapInterval((asset->cfg.graphics.vsync) ? 1 : 0);
  }

  input = new Input(window, &asset->cfg);


  GUI::init(&asset->cfg);
  Texture::init(&asset->cfg);
  /* Spider::init(); */
  /* Model::init(&asset->cfg); */
  Framebuffer::init(&asset->cfg);
  Framebuffer::printFramebufferLimits();

  if (isRefresh == State::INIT) {
    states.push(initState);

    // Create the correct state based on the enum given
    if (initState == State::MAINMENU)
      current = new MainMenu(asset, input);
    else if (initState == State::MASTER_THESIS) {
      current = new Master(asset, input);
    }
    /* } else if (initState == State::GAME) { */
    /*   current = new Game(asset, input); */
    /* } */
  }

  log("Engine: Initialized successfully...");
  return true;
}

/**
 * @brief
 *   This function is expected to be called each time you
 *   want the state to perform an update. A lot of functionality
 *   depends on this function as all the draw calls are performed
 *   within the states update function.
 *
 * @param deltaTime
 *   The time since last update call
 */
void Engine::updateState(float deltaTime) {
  if (current != NULL) {
    current->update(deltaTime);
  } else {
    error("Engine::Failed at updateState - Current == NULL");
  }
}

/**
 * @brief
 *   This is the callback that is called for each key that
 *   is pressed after glfwPollEvents()
 *
 *   This function calls the current state (if it exists)
 *   and expects it to return a state. It may change
 *   the state depending on what is returned
 *
 * @param key
 *   Glfw key code
 *
 * @param scan
 *
 * @param action
 *   Whether it was a key press or a key release
 *
 * @param mods
 *   Which mods are activated
 */
void Engine::keyboardCB(int key, int scan, int action, int mods) {
  if (current == nullptr)
    return error("Engine: Failed at keyboardCB - Current == nullptr");

  int stateBack = current->keyboardCB(key, scan, action, mods);
  if (stateBack != states.top() && stateBack != State::NOCHANGE)
    changeState(stateBack);
}

/**
 * @brief
 *   This is the callback that is called when the mouse
 *   is moved after glfwPollEvents()
 *
 * @param x
 *   X position of the mouse in screen position
 *
 * @param y
 *   Y position of the mouse in screen position
 */
void Engine::mouseMovementCB(double x, double y) {
  if (current == nullptr)
    return error("Engine: Failed at mouseMovementCB - Current == nullptr");

  current->mouseMovementCB(x, y);
}

/**
 * @brief
 *   This is called whenever glfw detects a mouse click after
 *   glfwPollEvents is called
 *
 * @psaram button
 *   Which mouse button was clicked
 *
 * @param action
 *   Whether it was release or press
 *
 * @param mods
 *   Which mod keys are pressed
 */
void Engine::mouseButtonCB(int button, int action, int mods) {
  if (current == nullptr) {
    return error("Engine: Failed at mouseButtonCB - CURRENT == nullptr");
  }

  int stateBack = current->mouseButtonCB(button, action, mods);
  if (stateBack != states.top() && stateBack != State::NOCHANGE)
    changeState(stateBack);
}

/**
 * @brief
 *   This is called whenever the user scrolls after
 *   glfwPollEvents() is called.
 *
 * @param offsetx
 * @param offsety
 */
void Engine::mouseScrollCB(double offsetx, double offsety) {
  if (current == nullptr)
    return error("Engine: Failed at mouseScrollCB - Current == NULL");

  current->mouseScrollCB(offsetx, offsety);
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
    error("Engine: Could not start GLFW - Check GLFW error");
    return false;
  }

  log("Engine: GLFW started successfully");
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
    error("Engine: Failed to initialize OGL: ");
    return false;
  }

  // check for opengl errors
  if (!Utils::getGLError())
    return false;

  log("Engine: OGL initialized successfully...");
  return true;
}

bool Engine::initWindow() {
  CFG *cfg = &asset->cfg;

  // Set version we want
  glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
  glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);

  glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);

  // Set the debug context, since we are developing
  glfwWindowHint(GLFW_OPENGL_DEBUG_CONTEXT, GL_TRUE);
  glfwWindowHint(GLFW_SAMPLES, cfg->graphics.aliasing);

  // No resizing, because of the resolutions in settings
  glfwWindowHint(GLFW_RESIZABLE, GL_FALSE);

  // If window mode, do decorated
  glfwWindowHint(GLFW_DECORATED,
                 cfg->graphics.winMode == 1 ? GL_FALSE : GL_TRUE);

  // get monitor, may be null, but thats okay since glfw supports it
  GLFWmonitor *monitor = getMonitor();
  window = glfwCreateWindow(cfg->graphics.res.x, cfg->graphics.res.y, "DDDGP",
                            monitor, NULL);

  if (!window) {
    error("Engine: Could not open window with GLFW. Check GLFW error messages");
    glfwTerminate();
    return false;
  }

  // Make the window our openGL context
  glfwMakeContextCurrent(window);

  // Set vsync if set by config
  glfwSwapInterval((cfg->graphics.vsync) ? 1 : 0);

  log("Engine: GLFW Window settings initalized successfully...");
  return true;
}

/**
 * @brief
 *   Initialize OpenGL with the settings that we need.
 */
void Engine::initGL() {
  glViewport(0, 0, asset->cfg.graphics.res.x, asset->cfg.graphics.res.y);
  glEnable(GL_BLEND);
  glEnable(GL_BLEND);
  glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

  glEnable(GL_DEPTH_TEST);
  glDepthFunc(GL_LESS);

  glEnable(GL_CULL_FACE);
  glCullFace(GL_BACK);
  glFrontFace(GL_CCW);
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
GLFWmonitor *Engine::getMonitor() {
  int monitorCount;
  GLFWmonitor *monitor         = NULL;
  const GLFWvidmode *videoMode = NULL;
  CFG cfg                      = asset->cfg;
  GLFWmonitor **monitors       = glfwGetMonitors(&monitorCount);

  // if we are running fullscreen, we gotta use the correct monitor
  if (cfg.graphics.winMode == 2) {

    // if the user has selected a monitor and its
    // below the max number of monitors we use it,
    // otherwise use primary
    if (cfg.graphics.monitor < monitorCount)
      monitor = monitors[cfg.graphics.monitor];
    else
      monitor = glfwGetPrimaryMonitor();

    // since its fullscreen, we get the resolution of the monitor
    videoMode = glfwGetVideoMode(monitor);
    cfg.graphics.res = vec2(videoMode->width, videoMode->height);
  }

  return monitor;
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
  GUI::deinit();
  /* Model::deinit(); */
  /* Spider::deinit(); */
  Framebuffer::deinit();

  asset->cfg.writetoFile("config/config.ini");
  delete asset;
  delete input;

  if (isFullDeinit)
    glfwTerminate();
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

  while (!glfwWindowShouldClose(window)) {
    float currentTime = glfwGetTime();
    float deltaTime = currentTime - startTime;
    startTime = currentTime;

    LOOP_LOGGER += deltaTime;
    updateState(deltaTime);

    glfwPollEvents();
    glfwSwapBuffers(window);

    if (LOOP_LOGGER > 1) {
      LOOP_LOGGER = 0;
    }
  }
}

/**
 * @brief
 *   Refreshes the state by checking the new configuration that might have been
 *   added and sets those.
 *
 * @param isWinRefresh
 *   When true, it means that some changes to the configuration was made so
 *   that a normal refresh isnt good enough. If true, it will unload all
 *   resources before reloading them.
 */
void Engine::refreshState(bool isWinRefresh) {
  int type = isWinRefresh ? State::WINREFRESH : State::REFRESH;
  deinitialize(isWinRefresh);

  if (!initialize(0, NULL, type)) {
    throw Error("Engine failed to refresh!");
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
    case State::QUITALL:
      while (!states.empty())
        states.pop();
      return;
    case State::QUIT:
      states.pop();
      break;
    case State::REFRESH:
      refreshState(false);
      break;
    case State::WINREFRESH:
      refreshState(true);
      break;
    default:
      states.push(newState);
      break;
  }
  createState();
}

/**
 * @brief
 *   Creates the state that is at the top of the state stack. Deletes
 *   the current state if that exists
 */
void Engine::createState() {
  if (current != nullptr) {
    delete current;
    current = nullptr;
  }

  if (states.size() == 0) {
    closeWindow();
    return;
  }

  switch (states.top()) {
    case State::MAINMENU: current = new MainMenu(asset, input); break;
    /* case State::GAME    : current = new Game(asset, input); break; */
    case State::MASTER_THESIS: current = new Master(asset, input); break;
    default:
      error("Tried to make state that does not exist: ", states.top());
      closeWindow();
      return;
  }
}

/**
 * @brief
 *   Tells glfw to close the window. Since the loop checks for a flag which is
 *   called `windowShouldClose`, calling this function effectively ends the
 *   main loop and therefore stops the program
 */
void Engine::closeWindow() {
  log("Now closing GLFW window..");
  glfwSetWindowShouldClose(window, GL_TRUE);
}