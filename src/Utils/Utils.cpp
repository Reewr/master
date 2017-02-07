#include "Utils.hpp"

#include <chrono>
#include <ctime>

#include "../OpenGLHeaders.hpp"

bool  DEBUG_MODE    = false;
bool  ENABLE_COLORS = false;
float LOOP_LOGGER   = 1000;

static auto START     = std::chrono::high_resolution_clock::now();
static int  MS_SECOND = 1000;
static int  MS_MINUTE = 60 * MS_SECOND;

std::string timeSinceStart() {
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

  std::string sMinutes = (mins < 10 ? "0" : "") + std::to_string(mins) + "m";
  std::string sSeconds = (secs < 10 ? "0" : "") + std::to_string(secs) + "s";
  std::string sMs =
    (ms < 10 ? "00" : ms < 100 ? "0" : "") + std::to_string(ms) + "ms";
  std::string sMicro =
    (micro < 10 ? "00" : micro < 100 ? "0" : "") + std::to_string(micro) + "μs";

  return "[" + sMinutes + " " + sSeconds + " " + sMs + " " + sMicro + "]";
}

void Utils::logTimeNoEnd() {
  std::cout << timeSinceStart() << " ";
}

bool Utils::getGLError(const std::string& place) {
  GLenum glError = glGetError();
  if (glError != GL_NO_ERROR) {
    error("OpenGL Error after: " + place + " ", glError);
    // const unsigned char* errorChar = gluErrorString(glerror);
    return false;
  }
  return true;
}

void Utils::clearGLError() {
  glGetError();
}

void fatalError() {
  std::cout << "Fatal error!" << std::endl;
}

void error() {
  if (DEBUG_MODE)
    std::cout << "Error!" << std::endl;
}

void warning() {
  if (DEBUG_MODE)
    std::cout << "Warning!" << std::endl;
}

void log() {
  if (DEBUG_MODE)
    std::cout << std::endl;
}

void Utils::lineLog() {}
void _utils_detail::lineLog() {}

void Utils::logPercent(int percent, std::string msg) {
  if (percent == 100) {
    std::cout << '\r';
    return;
  }

  std::string n = (percent < 10) ? "  " : (percent < 100) ? " " : "";
  n += std::to_string(percent);
  std::string m = n + "% [" + std::string(percent / 5, '=') +
                  std::string(20 - percent / 5, ' ') + "]";
  m += (msg != "") ? " - " + msg : "";
  lineLog(m);
}

void tlog() {
  if (LOOP_LOGGER > 1)
    log();
}