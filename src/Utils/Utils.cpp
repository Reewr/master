#include "Utils.hpp"
#include "../GlobalLog.hpp"

#include <chrono>
#include <ctime>
#include <assert.h>

#include "../OpenGLHeaders.hpp"

float LOOP_LOGGER = 1000;

static auto START     = std::chrono::high_resolution_clock::now();
static int  MS_SECOND = 1000;
static int  MS_MINUTE = 60 * MS_SECOND;

std::string Utils::timeSinceStart() {
  auto finish = std::chrono::high_resolution_clock::now();
  int  micro =
    std::chrono::duration_cast<std::chrono::microseconds>(finish - START)
      .count();

  int ms = (double) micro / 1000.0;
  micro -= ms * 1000;
  int mins = (double) ms / (double) MS_MINUTE;
  ms -= mins * MS_MINUTE;

  int secs = (double) ms / (double) MS_SECOND;
  ms -= secs * MS_SECOND;

  return (mins < 10 ? "0" : "") + std::to_string(mins) + ":" +
         (secs < 10 ? "0" : "") + std::to_string(secs) + "." +
         (ms < 10 ? "00" : ms < 100 ? "0" : "") + std::to_string(ms);
}

bool Utils::getGLError(const std::string& place) {
  GLenum glError = glGetError();
  if (glError != GL_NO_ERROR) {
    error("OpenGL Error after: '{}': {}", place, glError);
    // const unsigned char* errorChar = gluErrorString(glerror);
    return false;
  }
  return true;
}

void Utils::assertGL() {
  GLenum glError = glGetError();

  if (glError != GL_NO_ERROR) {
    error("OpenGL Error: {}", glError);
  }

  assert(glError == GL_NO_ERROR);
}

void Utils::clearGLError() {
  glGetError();
}
