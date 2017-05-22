#include "Utils.hpp"
#include "../GlobalLog.hpp"

#include <assert.h>
#include <chrono>
#include <ctime>

#include "../OpenGLHeaders.hpp"

float LOOP_LOGGER = 1000;

static auto START     = std::chrono::high_resolution_clock::now();
static int  MS_SECOND = 1000;
static int  MS_MINUTE = 60 * MS_SECOND;

/**
 * @brief
 *   Returns the time since start of the program as a string
 *   with MM:SS:MS where MM is minutes, SS is seconds,
 *   MS is milliseconds.
 *
 * @param includeMicro
 *   if true, it will add micro seconds to the back of the string.
 *
 * @return
 */
std::string Utils::timeSinceStart(bool includeMicro) {
  auto                   finish = std::chrono::high_resolution_clock::now();
  unsigned long long int micro =
    std::chrono::duration_cast<std::chrono::microseconds>(finish - START)
      .count();

  unsigned long long int ms = (double) micro / 1000.0;
  micro -= ms * 1000;

  unsigned int mins = (double) ms / (double) MS_MINUTE;
  ms -= mins * MS_MINUTE;

  unsigned int secs = (double) ms / (double) MS_SECOND;
  ms -= secs * MS_SECOND;

  if (!includeMicro) {
    return (mins < 10 ? "0" : "") + std::to_string(mins) + ":" +
           (secs < 10 ? "0" : "") + std::to_string(secs) + "." +
           (ms < 10 ? "00" : ms < 100 ? "0" : "") + std::to_string(ms);
  }

  return (mins < 10 ? "0" : "") + std::to_string(mins) + ":" +
         (secs < 10 ? "0" : "") + std::to_string(secs) + "." +
         (ms < 10 ? "00" : ms < 100 ? "0" : "") + std::to_string(ms) + "-" +
         (micro < 10 ? "00" : micro < 100 ? "0" : "") + std::to_string(micro) +
         "us";
}

/**
 * @brief
 *   Returns false if there are errors, true if there are none.
 *   Prints out the error if there are any,
 *
 * @param place
 *
 * @return
 */
bool Utils::getGLError(const std::string& place) {
  GLenum glError = glGetError();
  if (glError != GL_NO_ERROR) {
    error("OpenGL Error after: '{}': {}", place, glError);
    // const unsigned char* errorChar = gluErrorString(glerror);
    return false;
  }
  return true;
}

/**
 * @brief
 *   If there are any errors on in GL, it will crash the program,
 *   giving you a stack trace so that you can see where it happened.
 */
void Utils::assertGL() {
  GLenum glError = glGetError();

  if (glError != GL_NO_ERROR) {
    error("OpenGL Error: {}", glError);
  }

  assert(glError == GL_NO_ERROR);
}

/**
 * @brief
 *   Calls getGLError without doing anything, which just clears
 *   the error from the OpenGL state
 */
void Utils::clearGLError() {
  glGetError();
}
