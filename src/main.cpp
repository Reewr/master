#include <backward.hpp>

#include "Engine.hpp"
#include "GlobalLog.hpp"
#include "Log.hpp"
#include <stdexcept>

Engine* engine = nullptr;
// Handle sigterm on both Linux and Windows
#if defined(WIN32) || defined(_WIN32) || defined(__WIN32) || defined(__CYGWIN__)
#include <windows.h>
BOOL WINAPI consoleHandler(DWORD signal) {
  if (signal == CTRL_C_EVENT)
    engine->closeWindow();
}
#else
#include <cstdlib>
#include <signal.h>

void controlCHandler(int) {
  engine->closeWindow();
}
#endif

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

  // Add the control C handler
#if defined(WIN32) || defined(_WIN32) || defined(__WIN32) || defined(__CYGWIN__)
  if (!SetConsoleCtrlHandler(consoleHandler, TRUE))
    throw std::runtime_error("Unable to set Console Handler");
#else
  signal(SIGINT, controlCHandler);
#endif

  Logging::init(spdlog::level::trace);

  engine = new Engine();

  if (!engine->initialize(argc, argv, States::Init, States::MasterThesis)) {
    throw std::runtime_error("Engined failed to initialize");
  }

  // runs the loop, exiting on any errors
  engine->runLoop();

  delete engine;

  return 0;
}
