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

  DEBUG_MODE    = true;
  ENABLE_COLORS = true;

  error("Testing");
  warning("Testing");
  log("Starting!");
  Engine* engine = new Engine();

  if (!engine->initialize(argc, argv)) {
    throw std::runtime_error("Engined failed to initialize");
  }

  // runs the loop, exiting on any errors
  engine->runLoop();

  delete engine;

  return 0;
}
