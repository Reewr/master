#include "Utils.hpp"

#include <algorithm>
#include <stack>
#include <cstdio>
#include <ctime>
#include <fstream>
#include <sstream>

#include "../OpenGLHeaders.hpp"

bool* DEBUG_MODE;
bool* ENABLE_COLORS;
float LOOP_LOGGER = 1000;

static std::clock_t START = std::clock();
static int MS_SECOND = 1000;
static int MS_MINUTE = 60*MS_SECOND;
static int MS_HOUR   = 60*MS_MINUTE;

/* Utils::Time::Time(double s) { */
/*   ms = s * 1000; */
/*   seconds = 0; */
/*   minutes = 0; */
/*   hours = 0; */
/*   while (ms >= 1000) { */
/*     if (ms >= 3600000) { */
/*       hours += 1; */
/*       ms -= 3600000; */
/*     } else if (ms >= 60000) { */
/*       minutes += 1; */
/*       ms -= 60000; */
/*     } else if (ms >= 1000) { */
/*       seconds += 1; */
/*       ms -= 1000; */
/*     } */
/*   } */
/* } */

/* std::string Utils::Time::toString() { */
/*   std::string t = "["; */
/*   t += Utils::toStr(hours) + "h" + ((minutes < 10) ? " 0" : " "); */
/*   t += Utils::toStr(minutes)  + "m" + ((seconds < 10) ? " 0" : " "); */
/*   t += Utils::toStr(seconds)  + "s" + ((ms < 10) ? " 00" : (ms < 100) ? " 0" : " "); */
/*   t += Utils::toStr(ms)   + "ms] "; */
/*   return t; */
/* } */

/* void Utils::Time::log() { */
/*   std::cout << toString(); */
/* } */

std::string timeSinceStart() {
  double duration = (std::clock() - START) / (double) CLOCKS_PER_SEC;
  int ms = duration * 1000;
  int minutes = (double) ms / (double) MS_MINUTE;
  ms -= minutes * MS_MINUTE;

  int seconds = (double) ms / (double) MS_SECOND;
  ms -= seconds * MS_SECOND;

  std::string sMinutes = (minutes < 10 ? "0" : "") + Utils::toStr(minutes) + "m";
  std::string sSeconds = (seconds < 10 ? "0" : "") + Utils::toStr(seconds) + "s";
  std::string sMs = (ms < 10 ? "00" : ms < 100 ? "0" : "") + Utils::toStr(ms) + "ms";

  return "[" + sMinutes + " " + sSeconds + " " + sMs + "]";
}

/**
 * @brief
 *   Checks if a file exists, returns true
 *   if it does, otherwise false
 *
 * @param name
 *   the path to the file
 *
 * @return
 */
bool Utils::fileExists (const std::string& name) {
  std::ifstream f(name.c_str());
  bool isGood = f.good();
  f.close();
  return isGood;
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

void Utils::logTimeNoEnd() {
  std::cout << timeSinceStart() << " ";
}

void Utils::logTime(std::string loc) {
  log(loc + " - ", glfwGetTime(), " seconds.");
}

bool Utils::getGLError() {
  GLenum glerror = glGetError();
  if(glerror != GL_NO_ERROR) {
    //const unsigned char* errorChar = gluErrorString(glerror);
    //error("OpenGL Error: ", errorChar);
    return false;
  }
  return true;
}

void Utils::clearGLError() {
  glGetError();
}

void fatalError () {
  std::cout << "Fatal error!" << std::endl;
}

void error () {
  if (DEBUG_MODE && *DEBUG_MODE)
    std::cout << "Error!" << std::endl;
}

void warning () {
  if (DEBUG_MODE && *DEBUG_MODE)
    std::cout << "Warning!" << std::endl;
}

void log () {
  if (DEBUG_MODE && *DEBUG_MODE)
    std::cout << std::endl;
}

void Utils::lineLog() {}
void _utils_detail::lineLog() {}

void Utils::logPercent(int percent, std::string msg) {
  if(percent == 100) {
    std::cout << '\r';
    return;
  }

  std::string n = (percent < 10) ? "  " : (percent < 100) ? " " : "";
  n += Utils::toStr(percent);
  std::string m = n + "% [" + std::string(percent/5, '=') + std::string(20-percent/5, ' ') + "]";
  m += (msg != "") ? " - " + msg : "";
  lineLog(m);
}

void tlog () {
  if (LOOP_LOGGER > 1)
    log ();
}


std::string TEMP::getPath(int i) {
  switch(i) {
    case OPTSMENU: return "./media/Textures/optionsMenu.png";
    case DROPDOWN: return "./media/Textures/dropdown.png";
    case SLIDER  : return "./media/Textures/sliderbg.png";
    case SLIDERB : return "./media/Textures/sliderbutton.png";
    case SPACE   : return "./media/Textures/space1.png";
    case TERRAIN : return "./media/Textures/terrain.png";
    case TREE    : return "./media/models/tree2.dae";
    case BLACK   : return "./media/Textures/optionsMenu.png";
    case POWERI  : return "./media/Icons/Power.png";
    case ROCKI   : return "./media/Icons/Rock.png";
    case PLANKI  : return "./media/Icons/Planks.png";
    case COGSI   : return "./media/Icons/Cogs.png";
    case WIREI   : return "./media/Icons/wireframe.png";
    case XMLOPT  : return "./media/XML/GUI.xml";
    case XMLBUILD: return "./media/XML/GUIBuilding.xml";
    case XMLRES  : return "./media/XML/GUIResources.xml";
    case FONT    : return "./media/Fonts/neuropolitical.ttf";
    default:       return "./media/Textures/debug.png";
  }
  return "./media/Textures/debug.png";
}