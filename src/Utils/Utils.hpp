#pragma once

#include <iostream>
#include <string>

extern bool DEBUG_MODE;
extern bool ENABLE_COLORS;
extern float LOOP_LOGGER;

namespace Utils {

  bool getGLError(const std::string& place = "");
  void clearGLError();

  void logTimeStart(std::string name    = "TimeStart",
                    bool        isMS    = true,
                    bool        isPrint = true);
  double logTimeEnd();

  void glLogTimeStart(std::string name    = "glTimeStart",
                      bool        isMS    = true,
                      bool        isPrint = true);
  double glLogTimeEnd();

  void lineLog();
  template <typename... Ts>
  void lineLog(Ts... args);

  void logTimeNoEnd();
  void logPercent(int percent, std::string msg = "");
}

//! print fatal error regardless of DEBUG_MODE
void fatalError();
template <typename... Ts>
void fatalError(Ts... args);

//! prints error to standard er if DEBUG_MODE is on
void error();
template <typename... Ts>
void error(Ts... args);

//! prints warning to standard err if DEBUG_MODE is on
void warning();
template <typename... Ts>
void warning(Ts... args);

//! prints to standard out if DEBUG_MODE is on
void log();
template <typename... Ts>
void log(Ts... args);

void tlog();
template <typename... Ts>
void tlog(Ts... args);

#include "Utils.tpp"