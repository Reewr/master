#include "Utils.hpp"

#include <algorithm>
#include <chrono>
#include <cstdio>
#include <ctime>
#include <fstream>
#include <sstream>
#include <stack>

#include "../OpenGLHeaders.hpp"

bool* DEBUG_MODE;
bool* ENABLE_COLORS;
float LOOP_LOGGER = 1000;

static auto START     = std::chrono::high_resolution_clock::now();
static int  MS_SECOND = 1000;
static int  MS_MINUTE = 60 * MS_SECOND;

static std::map<unsigned int, std::string> utf8Characters = {};

std::string timeSinceStart() {
  auto   finish = std::chrono::high_resolution_clock::now();
  double micro =
    std::chrono::duration_cast<std::chrono::microseconds>(finish - START)
      .count();

  int ms = (double) micro / 1000.0;
  micro -= ms * 1000;
  int mins = (double) ms / (double) MS_MINUTE;
  ms -= mins * MS_MINUTE;

  int secs = (double) ms / (double) MS_SECOND;
  ms -= secs * MS_SECOND;

  std::string sMinutes = (mins < 10 ? "0" : "") + Utils::toStr(mins) + "m";
  std::string sSeconds = (secs < 10 ? "0" : "") + Utils::toStr(secs) + "s";
  std::string sMs =
    (ms < 10 ? "00" : ms < 100 ? "0" : "") + Utils::toStr(ms) + "ms";
  std::string sNano = Utils::toStr(micro) + "μs";

  return "[" + sMinutes + " " + sSeconds + " " + sMs + " " + sNano + "]";
}

/**
 * @brief
 *   Turns the string `s` to lowercase, ie. "aAa" becomes
 *   "aaa".
 *
 * @param s
 *   The string to lowercase.
 *
 * @return
 *   Lowercased string
 */
std::string Utils::toLower(std::string s) {
  std::transform(s.begin(), s.end(), s.begin(), tolower);
  return s;
}

/**
 * @brief
 *   Turns the string `s` to uppercase, ie. "aAa" becomes
 *   "AAA".
 *
 * @param s
 *   The string to upper case.
 *
 * @return
 *   Uppercased string
 */
std::string Utils::toUpper(std::string s) {
  std::transform(s.begin(), s.end(), s.begin(), toupper);

  return s;
}

/**
 * @brief
 *   Takes an unsigned integer that represent a unicode character
 *   and turns it into a string. This function uses a map to cache
 *   the answer so it can be run multiple times on the same integer
 *   without the performance hit.
 *
 * @param codepoint
 *
 * @return
 */
std::string Utils::utf8toStr(unsigned int codepoint) {
  if (utf8Characters.count(codepoint))
    return utf8Characters[codepoint];

  std::string out;

  // this code has been taken from:
  // https://stackoverflow.com/questions/19968705/unsigned-integer-as-utf-8-value
  if (codepoint <= 0x7f)
    out.append(1, static_cast<char>(codepoint));
  else if (codepoint <= 0x7ff) {
    out.append(1, static_cast<char>(0xc0 | ((codepoint >> 6) & 0x1f)));
    out.append(1, static_cast<char>(0x80 | (codepoint & 0x3f)));
  } else if (codepoint <= 0xffff) {
    out.append(1, static_cast<char>(0xe0 | ((codepoint >> 12) & 0x0f)));
    out.append(1, static_cast<char>(0x80 | ((codepoint >> 6) & 0x3f)));
    out.append(1, static_cast<char>(0x80 | (codepoint & 0x3f)));
  } else {
    out.append(1, static_cast<char>(0xf0 | ((codepoint >> 18) & 0x07)));
    out.append(1, static_cast<char>(0x80 | ((codepoint >> 12) & 0x3f)));
    out.append(1, static_cast<char>(0x80 | ((codepoint >> 6) & 0x3f)));
    out.append(1, static_cast<char>(0x80 | (codepoint & 0x3f)));
  }

  utf8Characters[codepoint] = out;

  return out;
}

/**
 * @brief
 *   Trims the string from the right, which means any trailing whitespace,
 *   such that "  Hello World!   " becomes "  Hello World!"
 *
 *   Note: the string is sent by reference and does change the contents
 *
 * @param s
 * @returns s
 */
void Utils::rTrim(std::string& s) {
  s.erase(std::find_if(s.rbegin(),
                       s.rend(),
                       std::not1(std::ptr_fun<int, int>(std::isspace)))
            .base(),
          s.end());
}

/**
 * @brief
 *   Trims the string from the left.
 *   such that "  Hello World!  " becomes "Hello World!  "
 *
 *   Note: the string is sent by reference and does change the contents
 *
 * @param s
 * @returns s
 */
void Utils::lTrim(std::string& s) {
  s.erase(s.begin(),
          std::find_if(s.begin(),
                       s.end(),
                       std::not1(std::ptr_fun<int, int>(std::isspace))));
}

/**
 * @brief
 *   Trims the string from both left and right
 *   such that "  Hello World!  " becomes "Hello World!"
 *
 *   Note: the string is sent by reference and does change the contents
 *
 * @param s
 */
void Utils::trim(std::string& s) {
  rTrim(s);
  lTrim(s);
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
  if (DEBUG_MODE && *DEBUG_MODE)
    std::cout << "Error!" << std::endl;
}

void warning() {
  if (DEBUG_MODE && *DEBUG_MODE)
    std::cout << "Warning!" << std::endl;
}

void log() {
  if (DEBUG_MODE && *DEBUG_MODE)
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
  n += Utils::toStr(percent);
  std::string m = n + "% [" + std::string(percent / 5, '=') +
                  std::string(20 - percent / 5, ' ') + "]";
  m += (msg != "") ? " - " + msg : "";
  lineLog(m);
}

void tlog() {
  if (LOOP_LOGGER > 1)
    log();
}