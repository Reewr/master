#define SOL_SAFE_USERTYPE 1
#define SOL_CHECK_ARGUMENTS 1
#define BACKWARD_HAS_DW 1
#include "OpenGLHeaders.hpp"
#include <backward.hpp>

#include "Engine.hpp"
#include "Utils/Utils.hpp"

/**
 * @brief
 *   The main function of the entire game. It starts
 *   by creating an engine and initializing it. Most
 *   of the work is done by the Engine class itself.
 *
 *   The only thing this function is responsible for
 *   is starting the loop
 *
 *   The program accepts arguments that are considered
 *   to be configuration arguments. For instance,
 *   you can tell the game to start at a certain
 *   resolution or with a certain keybinding set.
 *
 * @param argc
 *   Number of arguments sent
 *
 * @param argv[]
 *   The arguments themselves
 *
 * @return
 *   Error code if any
 */
int main(int argc, char* argv[]) {

  DEBUG_MODE    = new bool(true); //&asset->cfg.general.debug;
  ENABLE_COLORS = new bool(true);

  error("Testing");
  warning("Testing");
  log("Starting DDDGP");
  Engine* engine = new Engine();

  try {
    if (!engine->initialize(argc, argv)) {
      throw Error("Engined failed to initialize");
    }
  } catch (const Error& err) {
    fatalError("Init :: Threw an 'Error': ", err.what());
    delete engine;
    return 1;
  } catch (const char* s) {
    fatalError("Init :: Threw an const char* Error: ", s);
    delete engine;
    return 1;
  } catch (std::string s) {
    fatalError("Init :: Threw an std::string error: ", s);
    delete engine;
    return 1;
  }

  // runs the loop, exiting on any errors
  try {
    engine->runLoop();
  } catch (const Error& err) {
    fatalError("Loop :: Threw an 'Error': ", err.what());
    delete engine;
    return 1;
  } catch (const char* s) {
    fatalError("Loop :: Threw an const char* Error: ", s);
    delete engine;
    return 1;
  } catch (std::string s) {
    fatalError("Loop :: Threw an std::string error: ", s);
    delete engine;
    return 1;
  }

  delete engine;

  return 0;
}
