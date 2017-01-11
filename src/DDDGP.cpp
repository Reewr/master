#include "OpenGLHeaders.hpp"

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
    fatalError(err.what());
    delete engine;
    return 1;
  } catch (const char* s) {
    fatalError("Threw an Error: ", s);
    delete engine;
    return 1;
  } catch (std::string s) {
    fatalError("Threw an Error: ", s);
    delete engine;
    return 1;
  }

  // runs the loop, exiting on any errors
  try {
    engine->runLoop();
  } catch (const Error& err) {
    fatalError(err.what());
    delete engine;
    return 1;
  } catch (const char* s) {
    fatalError("Threw an Error: ", s);
    delete engine;
    return 1;
  } catch (std::string s) {
    fatalError("Threw an Error: ", s);
    delete engine;
    return 1;
  }

  delete engine;

  return 0;
}
